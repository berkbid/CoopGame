// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/HorizontalBox.h"
#include "SHorizontalBoxInventory.generated.h"


enum class EAmmoType : uint8;

/**
 * 
 */
UCLASS()
class COOPGAME_API USHorizontalBoxInventory : public UHorizontalBox
{
	GENERATED_BODY()
	


public:

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	USHorizontalBoxInventory();

	/**
	 * Applies all properties to the native widget if possible.  This is called after a widget is constructed.
	 * It can also be called by the editor to update modified state, so ensure all initialization to a widgets
	 * properties are performed here, or the property and visual state may become unsynced.
	 */
	virtual void SynchronizeProperties() override;

	/* Handle visuals for picking up weapon */
	void HandlePickupWeapon(int32 WeaponSlot, class UTexture2D* WeaponTexture, int32 CurrentAmmo, int32 ExtraAmmo, EAmmoType NewAmmoType);

	/* Handle visuals for changing to new inventory slot */
	void HandleSlotChange(int32 WeaponSlot);

	/* Set ammo data for slot */
	void UpdateCurrentSlotAmmo(int32 CurrentAmmo);

	void UpdateExtraSlotAmmo(int32 ExtraAmmo);

	void UpdateBothAmmo(int32 CurrentAmmo, int32 ExtraAmmo);

	void UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo);

protected:

	/* Hold array of inventory slots */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Children")
	TArray<class USOverlayInventorySlot*> InventorySlots;

	/* Keep track of currently active slot locally */
	class USOverlayInventorySlot* CurrentSlot;
};
