// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Pickups/SItemPickup.h"
#include "SWeaponPickup.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASWeaponPickup : public ASItemPickup
{
	GENERATED_BODY()
	


public:
	ASWeaponPickup();

	virtual void PostInitProperties() override;

	/* Set these values in the child classes for specific weapon pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;



protected:

	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

	class UMaterialInstanceDynamic* MatInst;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Material")
	uint8 MaterialIndexToChange;
};
