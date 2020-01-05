// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Devices/SContainer.h"
#include "SChest.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASChest : public ASContainer
{
	GENERATED_BODY()


public:
	ASChest();

	virtual void BeginPlay() override;

	virtual void Interact(class APlayerController* InteractedPC) override;

	virtual void ShowItemInfo(bool bIsVisible) override;


protected:

	UPROPERTY(EditAnywhere, Category = "Items")
	uint8 NumberOfWeapons;

	UPROPERTY(EditAnywhere, Category = "Items")
	uint8 NumberOfAmmo;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FWeaponDropChance> WeaponDrops;

	UPROPERTY(EditAnywhere, Category="Items")
	TArray<TSubclassOf<class ASWeaponPickup>> AssaultRifleArray;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<TSubclassOf<class ASWeaponPickup>> GrenadeLauncherArray;

	/* Called when PlayerController interacts with us and opens chest */
	void SpawnWeapons();

	/* Helper function to spawn weapon of a class with a position offset */
	void SpawnNewWeapon(TSubclassOf<class ASWeaponPickup> NewWeaponPickup, float HorizontalOffset);

	virtual void OnRep_OpenContainer() override;

	int32 WeightSum;
};
