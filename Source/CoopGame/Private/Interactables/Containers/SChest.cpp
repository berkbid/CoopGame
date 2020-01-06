// Fill out your copyright notice in the Description page of Project Settings.


#include "SChest.h"
#include "SWeaponPickup.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

ASChest::ASChest()
{
	bIsOpened = false;
	WeightSum = 0;
}

void ASChest::BeginPlay()
{
	Super::BeginPlay();

	// Get weighted sum information
	for (FWeaponDropChance WeaponDrop : WeaponDrops)
	{
		WeightSum += WeaponDrop.DropChance;
	}
}

// We are server in here
void ASChest::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

	// Only allow chest to be opened once
	if (bIsOpened) { return; }

	// Tell clients this chest has been opened
	bIsOpened = true;
	OnRep_OpenContainer();

	SpawnWeapons();
}

void ASChest::SpawnWeapons()
{
	// For each weapon we want to spawn
	//for (int32 i = 0; i < NumberOfWeapons; ++i)
	//{
		// Choose what type of weapon to spawn
		//int32 RandomChoice = UKismetMathLibrary::RandomInteger(WeightSum);
		float TempHorizontalOffset = -50.f;

		for (FWeaponDropChance WeaponDrop : WeaponDrops)
		{
			//RandomChoice -= WeaponDrop.DropChance;
			//if (RandomChoice < 0) { break; }

			switch (WeaponDrop.WeaponType)
			{
			case EWeaponType::AssaultRifle:
				if (AssaultRifleArray.Num() >= 5)
				{
					SpawnNewWeapon(AssaultRifleArray[4], TempHorizontalOffset);
				}
				
				break;
			case EWeaponType::GrenadeLauncher:
				if (GrenadeLauncherArray.Num() >= 5)
				{
					SpawnNewWeapon(GrenadeLauncherArray[4], TempHorizontalOffset);
				}
				break;
			default:
				break;
			}

			TempHorizontalOffset += 50.f;
		}
		// Choose what rarity of weapon type to spawn
		// Spawn weapon
	//}
}

void ASChest::SpawnNewWeapon(TSubclassOf<ASWeaponPickup> NewWeaponPickup, float HorizontalOffset)
{
	if (!NewWeaponPickup) { return; }
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;

	FVector SpawnLocation = GetActorLocation() - FVector(100.f, HorizontalOffset, 0.f);
	GetWorld()->SpawnActor<ASWeaponPickup>(NewWeaponPickup, SpawnLocation, FRotator(0.f, 90.f, 0.f), SpawnParams);

	HorizontalOffset += 50.f;
}


void ASChest::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
}

void ASChest::OnRep_OpenContainer()
{
	Super::OnRep_OpenContainer();
}