// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "SHorizontalBoxInventory.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USHorizontalBoxInventory : public UHorizontalBox
{
	GENERATED_BODY()
	


public:

	USHorizontalBoxInventory();


	/**
	 * Applies all properties to the native widget if possible.  This is called after a widget is constructed.
	 * It can also be called by the editor to update modified state, so ensure all initialization to a widgets
	 * properties are performed here, or the property and visual state may become unsynced.
	 */
	virtual void SynchronizeProperties() override;

	void HandlePickupWeapon(class UTexture2D* WeaponTexture, int WeaponSlot);

	void HandleSlotChange(int WeaponSlot);

protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Children")
	TArray<class USOverlayInventorySlot*> InventorySlots;


	class USOverlayInventorySlot* CurrentSlot;
};
