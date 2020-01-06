// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
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

	HealthBar = CreateDefaultSubobject<USWidgetCompHealthBar>("HealthBar");
	HealthBar->SetupAttachment(RootComponent);

	bDied = false;
}

// Called once on client and once on server
// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	}
}

void ASCharacter::OnRep_CurrentWeapon()
{
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
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

	ASPlayerController* PC = Cast<ASPlayerController>(GetController());
	if (PC)
	{
		CurrentWeapon->CurrentClipSize += PC->ReloadAmmoClip(CurrentWeapon->CurrentClipSize);
	}
}

// This is called through ServerChangeWeapons() in SPlayerCharacter so server runs this code for players
// AI could call this directly to change weapons
int32 ASCharacter::EquipWeaponClass(FWeaponInfo NewWeaponInfo, int32 NewWeaponSlot)
{
	int32 OldAmmoCount = -1;

	// Should never be called on client
	if (GetLocalRole() < ROLE_Authority)
	{
		return OldAmmoCount;
	}
	// Keep track of what slot we have equipped
	CurrentWeaponSlot = NewWeaponSlot;

	if (CurrentWeapon)
	{
		OldAmmoCount = CurrentWeapon->GetCurrentAmmo();

		CurrentWeapon->Destroy();
	}

	TSubclassOf<ASWeapon> NewWeaponClass = NewWeaponInfo.WeaponType;
	// If invalid new weapon class, destroy current weapon and don't try to equip new one
	// This is completely valid, we allow for this, this is swapping to empty inventory slot
	if (!NewWeaponClass)
	{
		CurrentWeapon = nullptr;
		OnRep_CurrentWeapon();

		return OldAmmoCount;
	}

	// Spawn a weapon because it exists in the inventory slot, and update HUD image
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	// Must set owner so they are awarded for damage done by the weapon
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;


	// Create On_Rep function for client to update HUD when weapon changes and play sound
	// Need client to have this "CurrentWeapon" variable set also to call StartFire() and StopFire()
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	OnRep_CurrentWeapon();

	if (CurrentWeapon)
	{
		// Pass current clip size info to the spawned weapon
		CurrentWeapon->SetCurrentClipSize(NewWeaponInfo.CurrentAmmo);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}

	return OldAmmoCount;
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
			// Tell player controller the remaining ammo in gun
			int32 RemainingAmmo = CurrentWeapon->GetCurrentAmmo();
			ASPlayerController* PC = Cast<ASPlayerController>(GetController());
			if (PC)
			{
				PC->SetCurrentSlotAmmo(RemainingAmmo);
			}

			CurrentWeapon->Destroy();
		}

		// While PlayerState is valid before we detach and destroy this pawn, update Death count
		if (ASPlayerState* PS = GetPlayerState<ASPlayerState>())
		{
			PS->AddDeath();
			PS->AddScore(-10.f);
		}

		bDied = true;
		// This will manually call this method for the server, meanwhile clients call the method when bDied gets changed & ReplicatedUsing=OnRepDeath()
		OnRep_Death();
	}
}

// Replicated actions for death
void ASCharacter::OnRep_Death()
{
	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	if (HealthBar)
	{
		HealthBar->SetVisibility(false);
	}
	// Detaches player from the actor, then actor destroys in 10 seconds
	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.f);
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

// Widget Component is always valid here, but WidgetInst is not
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