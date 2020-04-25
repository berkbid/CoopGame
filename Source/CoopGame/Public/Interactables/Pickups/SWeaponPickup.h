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

	/** Allow actors to initialize themselves on the C++ side after all of their components have been initialized, only called during gameplay */
	virtual void PostInitializeComponents() override;

	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void InitItemInfo(const class ASPlayerController* ClientController) override;

	/* Override interact functionality to call HandlePickupWeapon functionality */
	virtual void Interact(class APlayerController* InteractedPC) override;

	/* Sets current clip size of weapon pickup, called by player controller when spawning pickup */
	void SetWeaponInfo(const FWeaponInfo& NewWeaponInfo);


public:
	/* Set these values in the child classes for specific weapon pickups */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	FWeaponInfo WeaponInfo;

	// Replicate this value so clients can see current ammo in gun on widget display
	UPROPERTY(Replicated, EditInstanceOnly, BlueprintReadWrite, Category = "Weapon")
	int32 WeaponCurrentAmmo;
	
protected:
	virtual void HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor) override;

	/* Call function on controller passing WeaponInfo to pick us up, and destroy self if successful */
	void HandlePickupWeapon(class AController* NewPickupController, bool bDidInteract);

	// Client owned value, kept track of in InitItemInfo in order to shortcut future calls
	bool bIsInventoryFullTemp;

};
