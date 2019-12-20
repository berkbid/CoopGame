// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "SWidgetCompHealthBar.h"
#include "SPlayerController.h"
#include "SPlayerState.h"


// Sets default values
ASCharacter::ASCharacter()
{
	CurrentWeapon = nullptr;

	WeaponAttachSocketName = "WeaponSocket";

	// We do not want capsule to block the weapon trace, only the mesh of character
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	//GetCharacterMovement()->SetIsReplicated(true);
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
	if (Role == ROLE_Authority)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	}
	
}

// This only calls when an actual weapon is equipped
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

// Being called by server only
void ASCharacter::PickupWeapon(TSubclassOf<ASWeapon> NewWeaponClass, AActor* PickupActor)
{
	if (!NewWeaponClass) { return; }

	// If we could get a boolean from player controller if inventory is full or not we could skip doing more work
	// This means AI don't pickup weapons because they have different controller class
	ASPlayerController* PC = Cast<ASPlayerController>(GetController());

	if (PC)
	{
		if (PC->bIsInventoryFull) { return; }

		if (PC->PickedUpNewWeapon(NewWeaponClass))
		{
			PickupActor->Destroy();
		}
	}
}

// Only called on server because we only hooked this on the server
void ASCharacter::OnHealthChanged(USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// This was previously never run on the client because bDied replicates the same time HealthChanged event triggers on the client
	if (Health <= 0.f && !bDied)
	{
		if (CurrentWeapon)
		{
			// Destroy weapon server side
			CurrentWeapon->Destroy();
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


// This is called through ServerChangeWeapons() in SPlayerCharacter so server runs this code for players
// AI could call this directly to change weapons
void ASCharacter::ChangeWeapons(TSubclassOf<ASWeapon> NewWeaponClass, int NewWeaponSlot)
{
	// Should never be called on client
	if (Role < ROLE_Authority)
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}

	// If invalid new weapon class, destroy current weapon and don't try to equip new one
	// This is completely valid, we allow for this, this is swapping to empty inventory slot
	if (!NewWeaponClass)
	{
		CurrentWeapon = nullptr;
		//Call this for listen server
		OnRep_CurrentWeapon();

		return;
	}


	// Spawn a weapon because it exists in the inventory slot, and update HUD image
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Create On_Rep function for client to update HUD when weapon changes and play sound
	// Need client to have this "CurrentWeapon" variable set also to call StartFire() and StopFire()
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	//Call this for listen server
	OnRep_CurrentWeapon();

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
	}
}


void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME_CONDITION(ASCharacter, CurrentWeapon, COND_OwnerOnly);
	DOREPLIFETIME(ASCharacter, bDied);
}

// Only server runs this code
void ASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Wrong playername on first call
	ASPlayerState* PS = NewController->GetPlayerState<ASPlayerState>();
	if (PS && HealthBar)
	{
		// Update names manually for listen server here 
		//cannot multicast for initial possessed as no clients are connected yet when server is
		HealthBar->UpdateWidgetName(PS->GetPlayerName());
	}
}
