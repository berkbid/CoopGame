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

	/* Set these values in the child classes for specific weapon pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;

	/* Override interact functionality to call HandlePickupWeapon functionality */
	virtual void Interact(class APlayerController* InteractedPC) override;

	
protected:

	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

	/* Call function on controller passing WeaponInfo to pick us up, and destroy self if successful */
	void HandlePickupWeapon(class AController* NewPickupController);
};