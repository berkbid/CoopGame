// Fill out your copyright notice in the Description page of Project Settings.


#include "SChest.h"
#include "SWeaponPickup.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"

ASChest::ASChest()
{
	bIsOpened = false;
	WeightSum = 0;
	HorizontalSpawnOffset = 75.f;
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
		StartingWeaponSpawnPos = (-HorizontalSpawnOffset) * (NumberOfWeapons / 2) + HorizontalSpawnOffset / 2;
	}
}

// We are server in here
void ASChest::Interact(APlayerController* InteractedPC)
{
	// Only allow chest to be opened once
	if (bIsOpened) { return; }

	// Tell clients this chest has been opened
	bIsOpened = true;
	OnRep_OpenContainer();

	SpawnWeapons();

	// Call super after interacted with to set dormant
	Super::Interact(InteractedPC);
}

void ASChest::SpawnWeapons()
{
	if (WeaponDrops.Num() <= 0) { return; }

	float TempHorizontalOffset = StartingWeaponSpawnPos;

	// For each weapon we want to spawn
	for (int32 i = 0; i < NumberOfWeapons; ++i)
	{
		// Choose what type of weapon to spawn
		int32 RandomChoice = UKismetMathLibrary::RandomInteger(WeightSum);
		EWeaponType WeaponTypeToSpawn = EWeaponType::AssaultRifle;

		// Find weapon type to spawn
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

		// Get pickup class to spawn using weapon type and rarity
		TSubclassOf<ASWeaponPickup> PickupClassToSpawn;
		switch (WeaponTypeToSpawn)
		{
		case EWeaponType::AssaultRifle:
			if (RifleArray.Num() > RarityIndex)
			{
				PickupClassToSpawn = RifleArray[RarityIndex];
			}
			break;
		case EWeaponType::GrenadeLauncher:
			if (GLArray.Num() > RarityIndex)
			{
				PickupClassToSpawn = GLArray[RarityIndex];
			}
			break;
		default:
			break;
		}

		SpawnItemFromClass(PickupClassToSpawn, 100.f, TempHorizontalOffset);
		TempHorizontalOffset += HorizontalSpawnOffset;
	}
}

void ASChest::SpawnItemFromClass(UClass* NewWeaponPickup, float ForwardOffset, float HorizontalOffset)
{
	if (!NewWeaponPickup) { return; }

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;


	FVector SpawnLocation = GetActorLocation() - FVector(ForwardOffset, HorizontalOffset, 0.f);
	GetWorld()->SpawnActor<AActor>(NewWeaponPickup, SpawnLocation, FRotator(0.f, 90.f, 0.f), SpawnParams);
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

void ASChest::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
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