// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "SWidgetCompHealthBar.h"
#include "SPlayerState.h"
#include "SWeapon.h"
#include "SPlayerController.h"

// Sets default values
ASCharacter::ASCharacter()
{
	CurrentWeapon = nullptr;

	WeaponAttachSocketName = "WeaponSocket";

	// We do not want capsule to block the weapon trace, only the mesh of character
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Needed to make sure we are allowed to crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComp = CreateDefaultSubobject<USHealthComponent>("HealthComp");
	HealthComp->SetIsReplicated(true);

	HealthBar = CreateDefaultSubobject<USWidgetCompHealthBar>("HealthBar");
	HealthBar->SetupAttachment(RootComponent);

	bDied = false;
}

void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	}
}

// This is called through ServerChangeWeapons() in SPlayerCharacter so server runs this code for players
// AI could call this directly to change weapons
void ASCharacter::EquipWeaponClass(TSubclassOf<ASWeapon> NewWeaponClass, const FWeaponStats &NewWeaponStats, int32 NewWeaponClipSize)
{
	// Should never be called on client
	if (GetLocalRole() < ROLE_Authority) { return; }

	// Destroy currently equipped weapon so we can equip new weapon
	if (CurrentWeapon) { CurrentWeapon->Destroy(); }

	// If invalid new weapon class, destroy current weapon and don't try to equip new one
	// This is completely valid, we allow for this, this is swapping to empty inventory slot
	if (!NewWeaponClass)
	{
		CurrentWeapon = nullptr;
		OnRep_CurrentWeapon();
		return;
	}

	// Spawn and attach weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// Must set owner so they are awarded for damage done by the weapon
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	// Need client to have this "CurrentWeapon" variable set also to call StartFire() and StopFire()
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	OnRep_CurrentWeapon();

	if (CurrentWeapon)
	{
		// Pass current clip size info to the spawned weapon, want to pass all NewWeaponInfo
		CurrentWeapon->InitWeaponState(NewWeaponStats, NewWeaponClipSize);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}

void ASCharacter::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
		return;
	}
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStopFire();
		return;
	}
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::Reload()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerReload();
		return;
	}

	if (!CurrentWeapon) { return; }

	// PlayerController is in charge of ammo inventory, so retrieve ammo by requesting from PC
	ASPlayerController* PC = Cast<ASPlayerController>(GetController());
	if (PC)
	{
		CurrentWeapon->CurrentClipSize += PC->ReloadAmmoClip();
	}
}

// Only called on server because we only hooked this on the server
void ASCharacter::OnHealthChanged(USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// This was previously never run on the client because bDied replicates the same time HealthChanged event triggers on the client
	if (Health <= 0.f && !bDied)
	{
		// Handle retrieving important info from current weapon then destroying it
		if (CurrentWeapon)
		{
			CurrentWeapon->Destroy();
		}

		// Run server code on player controller that owns this pawn upon reaching 0 health
		ASPlayerController* PC = Cast<ASPlayerController>(GetController());
		if (PC)
		{
			PC->HandlePawnDied();
			//if (DamageCauser)
			//{
			//	PC->SetViewTargetWithBlend(DamageCauser, 0.5f, VTBlend_Cubic);
			//}
			
		}

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		// Detaches player from the actor, then actor destroys in 5 seconds
		DetachFromControllerPendingDestroy();
		
		SetLifeSpan(5.f);
		// This replicates so clients can play death animation
		bDied = true;
	}
}

void ASCharacter::ServerStartFire_Implementation()
{
	StartFire();
}

void ASCharacter::ServerStopFire_Implementation()
{
	StopFire();
}

void ASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	SetWidgetName();

	// If we are the client who owns this pawn, call server function for listen server to update name as well
	if (IsLocallyControlled())
	{
		ServerSetWidgetName();
	}
}

void ASCharacter::OnRep_CurrentWeapon() {}

void ASCharacter::ServerReload_Implementation()
{
	Reload();
}

void ASCharacter::SetWidgetName()
{
	ASPlayerState* PS = Cast<ASPlayerState>(GetPlayerState());

	if (PS && HealthBar)
	{
		HealthBar->UpdateWidgetName(PS->GetPlayerName());
	}
}

// This is so we can run listen server functionality for OnRep_PlayerState
void ASCharacter::ServerSetWidgetName_Implementation()
{
	// Only listen server needs to update widget name
	ENetMode NetMode = GetNetMode();
	if (NetMode == NM_ListenServer)
	{
		SetWidgetName();
	}
}

// Only server runs this code, this is called after BeginPlay
void ASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		ENetMode NetMode = GetNetMode();
		if (NetMode == NM_ListenServer)
		{
			SetWidgetName();
		}
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASCharacter, CurrentWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ASCharacter, bDied);
}