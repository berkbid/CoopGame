// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPlayerController.h"
#include "Pickups/SItemPickup.h"
#include "SAmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASAmmoPickup : public ASItemPickup
{
	GENERATED_BODY()
	


public:
	ASAmmoPickup();

	/* Set these values in the child classes for specific weapon pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoAmount;

protected:
	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

};
