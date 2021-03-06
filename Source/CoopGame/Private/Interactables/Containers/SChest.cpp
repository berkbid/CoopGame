// Fill out your copyright notice in the Description page of Project Settings.


#include "SChest.h"
#include "SWeaponPickup.h"
#include "SAmmoPickup.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"

ASChest::ASChest()
{
	bIsOpened = false;
	WeightSum = 0;
	HorizontalSpawnOffset = 100.f;

	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
}

void ASChest::BeginPlay()
{
	Super::BeginPlay();

	// Only server needs to run this code because only server uses this data inside SpawnWeapons() method
	if (GetLocalRole() == ROLE_Authority)
	{
		// Gather initial data for SpawnWeapons() functionality

		// Get weighted sum information
		for (FWeaponPickup WeaponDrop : WeaponDrops)
		{
			WeightSum += WeaponDrop.DropChance;
		}

		// Calculate initial weapon spawn position
		float StartingWeaponOffset = (-HorizontalSpawnOffset) * (NumberOfWeapons / 2) + HorizontalSpawnOffset / 2;
		StartingSpawnLocation = GetActorLocation() - FVector(100.f, StartingWeaponOffset, 0.f);
	}
}

// We are server in here
void ASChest::Interact(APlayerController* InteractedPC)
{
	if (bIsOpened) { return; }
	if (!InteractedPC) { return; }

	// If we open chest, then spawn weapons and call parent Interact()
	SpawnWeapons();

	Super::Interact(InteractedPC);
}

void ASChest::SpawnWeapons()
{
	if (WeaponDrops.Num() <= 0) { return; }

	// For each weapon we want to spawn
	for (int32 i = 0; i < NumberOfWeapons; ++i)
	{
		// Choose what type of weapon to spawn
		int32 RandomChoice = UKismetMathLibrary::RandomInteger(WeightSum);
		EWeaponType WeaponTypeToSpawn = EWeaponType::AssaultRifle;

		// Find weapon type to spawn using weighted random algorithm
		for (const FWeaponPickup &WeaponDrop : WeaponDrops)
		{
			RandomChoice -= WeaponDrop.DropChance;
			if (RandomChoice < 0) 
			{ 
				WeaponTypeToSpawn = WeaponDrop.WeaponType;
				break; 
			}
		}

		// Find rarity of weapon type to spawn
		uint8 RarityIndex = ChooseRarity();

		// Get weapon and ammo pickup class to spawn using weapon type and rarity
		TSubclassOf<ASWeaponPickup> WeaponPickupClassToSpawn;
		TSubclassOf<ASAmmoPickup> AmmoPickupClassToSpawn;
		switch (WeaponTypeToSpawn)
		{
		case EWeaponType::AssaultRifle:
			if (RifleArray.Num() > RarityIndex)
			{
				WeaponPickupClassToSpawn = RifleArray[RarityIndex];
				AmmoPickupClassToSpawn = MediumAmmoPickupClass;
			}
			break;
		case EWeaponType::GrenadeLauncher:
			if (GLArray.Num() > RarityIndex)
			{
				WeaponPickupClassToSpawn = GLArray[RarityIndex];
				AmmoPickupClassToSpawn = RocketAmmoPickupClass;
			}
			break;
		default:
			break;
		}

		// Spawn weapon with matching ammo
		if (WeaponPickupClassToSpawn)
		{
			
			GetWorld()->SpawnActor<ASWeaponPickup>(WeaponPickupClassToSpawn, StartingSpawnLocation, FRotator(0.f, 90.f, 0.f), SpawnParams);
		}
		if (AmmoPickupClassToSpawn)
		{
			GetWorld()->SpawnActor<ASAmmoPickup>(AmmoPickupClassToSpawn, StartingSpawnLocation + FVector(0.f, -50.f, 5.f), FRotator(0.f, 180.f, 0.f), SpawnParams);
		}

		// Update spawn location for next weapon/ammo spawn
		StartingSpawnLocation.Y -= HorizontalSpawnOffset;
	}
}

uint8 ASChest::ChooseRarity()
{
	int32 RandomRarity = UKismetMathLibrary::RandomInteger(25);

	// Common index - 32% chance
	if (RandomRarity < 8)
	{
		return 0;
	}
	// UnCommon index - 28% chance
	else if (RandomRarity < 15)
	{
		return 1;
	}
	// Rare index - 20% chance
	else if (RandomRarity < 20)
	{
		return 2;
	}
	// Epic index - 12% chance
	else if (RandomRarity < 23)
	{
		return 3;
	}
	// Legendary index - 8% chance
	else 
	{
		return 4;
	}

	return 0;
}


void ASChest::OnRep_OpenContainer()
{
	Super::OnRep_OpenContainer();

	// Change color of chest material when it is opened

	if (!MatInst)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (!MatInst2)
	{
		MatInst2 = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(1, MeshComp->GetMaterial(1));
	}

	if (MatInst)
	{
		// This variable "LastTimeDamageTaken" is set in the editor in the graph for the material
		MatInst->SetVectorParameterValue("Color", FLinearColor(0.3f, 0.3f, 0.3f, 1.f));
	}
	if (MatInst2)
	{
		// This variable "LastTimeDamageTaken" is set in the editor in the graph for the material
		MatInst2->SetVectorParameterValue("Color", FLinearColor(0.3f, 0.3f, 0.3f, 1.f));
	}
}