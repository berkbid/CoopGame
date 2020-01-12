// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitItemInfo(const class ASPlayerController* ClientController) override;

	virtual void Interact(class APlayerController* InteractedPC) override;

	/* Set these values in the child classes for specific weapon pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	EAmmoType AmmoType;

	UPROPERTY(ReplicatedUsing=OnRep_AmmoAmount, EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 AmmoAmount;

	UFUNCTION()
	void OnRep_AmmoAmount();

	
protected:

	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

	void HandleAmmoPickup(class AController* NewPickupController);

	// Client owned value, kept track of in InitItemInfo in order to shortcut future calls
	bool bIsInventoryFullTemp;
};