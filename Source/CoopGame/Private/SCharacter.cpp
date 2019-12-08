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
	if (Role == ROLE_Authority)
	{
		HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
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

// Being called by server only
void ASCharacter::PickupWeapon(TSubclassOf<ASWeapon> NewWeaponClass)
{
	//int InventorySize = WeaponInventory.Num();
	// Don't pick up weapon if we already have 5 inventory items, don't pick up next item
	//if (InventorySize >= 5) { return; }

	if (!NewWeaponClass) { return; }
	
	// We can replicate this array, so when it is changed, client updates HUD appropriately
	// If we pick up a weapon, just add it to our weapon inventory if we have space!!!
	//WeaponInventory.Add(NewWeaponClass);


	// Don't need to change to the weapon
	// If we have 1 weapon in inventory, that is the 0th index weapon slot
	//ChangeWeapons(NewWeaponClass, WeaponInventory.Num() - 1);



	//if(WeaponInventory)
	//if (!FirstWeaponClass)
	//{
	//	// This class update gets replicated to owner only
	//	FirstWeaponClass = NewWeaponClass;
	//	ChangeWeapons(FirstWeaponClass, 0);
	//}
	//else
	//{
	//	SecondWeaponClass = NewWeaponClass;
	//	ChangeWeapons(SecondWeaponClass, 1);
	//}
	

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
	// When we call this on client, call it on server instead
	if (Role < ROLE_Authority)
	{
		ServerChangeWeapons(NewWeaponClass, NewWeaponSlot);
		return;
	}

	//CurrentSlot only used set and used by server
	//if (CurrentSlot == NewWeaponSlot) { return; }

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

		//Update HUD info on client now that we ACTUALLY attached a new weapon
		ClientSetHUD(NewWeaponClass, NewWeaponSlot);
	}
}

void ASCharacter::ClientSetHUD_Implementation(TSubclassOf<ASWeapon> WeaponClass, int WeaponSlot)
{
	if (ASPlayerController* PC = Cast<ASPlayerController>(GetController())) 
	{ 
		PC->SetCurrentWeapon(WeaponClass, WeaponSlot);
	}
}

// MUST prefix with Server and require _Implementation
void ASCharacter::ServerChangeWeapons_Implementation(TSubclassOf<ASWeapon> NewWeaponClass, int NewWeaponSlot)
{
	ChangeWeapons(NewWeaponClass, NewWeaponSlot);
}

bool ASCharacter::ServerChangeWeapons_Validate(TSubclassOf<ASWeapon> NewWeaponClass, int NewWeaponSlot)
{
	// This is for anti cheat stuff
	return true;
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
