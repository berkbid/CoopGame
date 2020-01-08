// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Interactables/Containers/SContainer.h"
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

	virtual void Interact(class APlayerController* InteractedPC) override;

	virtual void ShowItemInfo(bool bIsVisible) override;

protected:
	virtual void BeginPlay() override;

	/* Called when PlayerController interacts with us and opens chest */
	void SpawnWeapons();

	uint8 ChooseRarity();

	virtual void OnRep_OpenContainer() override;

	
protected:
	UPROPERTY(EditAnywhere, Category = "Items", meta = (ClampMin = 0, ClampMax = 10))
	uint8 NumberOfWeapons;

	UPROPERTY(EditAnywhere, Category = "Items")
	TArray<FWeaponPickup> WeaponDrops;

	UPROPERTY(EditDefaultsOnly, Category = "Items", meta = (ClampMin = 0.f, ClampMax = 200.f))
	float HorizontalSpawnOffset;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<TSubclassOf<class ASWeaponPickup>> RifleArray;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TArray<TSubclassOf<class ASWeaponPickup>> GLArray;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<class ASAmmoPickup> MediumAmmoPickupClass;

	UPROPERTY(EditDefaultsOnly, Category = "Items")
	TSubclassOf<class ASAmmoPickup> RocketAmmoPickupClass;

	class UMaterialInstanceDynamic* MatInst;
	
	class UMaterialInstanceDynamic* MatInst2;

	int32 WeightSum;

	FVector StartingSpawnLocation;

	FActorSpawnParameters SpawnParams;
};
