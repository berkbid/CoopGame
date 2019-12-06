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

	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	// Only runs this code on server
	// Spawn weapon on server, it is set to replicated in SWeapon.cpp thus will exist on clients as well
	if (Role == ROLE_Authority)
	{	
		ChangeWeapons(FirstWeaponClass, 1);
	}

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
	//CurrentSlot only used set and used by server
	if (CurrentSlot == NewWeaponSlot) { return; }

	// Don't try to change weapons if invalid new weapon class
	if (!NewWeaponClass) { return; }

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}

	// Spawn a default weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Need client to have this "CurrentWeapon" variable set also to call StartFire() and StopFire()
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(NewWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);

		// Update value of CurrentSlot so this function isn't called on the same weapon slot that is equipped
		CurrentSlot = NewWeaponSlot;
	}
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}
