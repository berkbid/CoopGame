// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
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

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	USHorizontalBoxInventory();

	/**
	 * Applies all properties to the native widget if possible.  This is called after a widget is constructed.
	 * It can also be called by the editor to update modified state, so ensure all initialization to a widgets
	 * properties are performed here, or the property and visual state may become unsynced.
	 */
	virtual void SynchronizeProperties() override;

	/* Handle visuals for picking up weapon, return true if picked up weapon in current slot */
	bool HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo, class UTexture2D* WeaponTexture);

	/* Handle visuals for changing to new inventory slot */
	void HandleSlotChange(int32 WeaponSlot, FWeaponInfo& NewWeaponInfo, int32& ExtraAmmo);

	/* Returns extra ammo for current slot */
	int32 GetExtraAmmoOfType(EAmmoType NewAmmoType);

	/* Set ammo data for slot */
	void UpdateCurrentSlotAmmo(int32 CurrentAmmo);

	/* Set ammo data for slot */
	void UpdateExtraSlotAmmo(int32 ExtraAmmo);

	/* Set ammo data for slot */
	void UpdateBothAmmo(int32 CurrentAmmo, int32 ExtraAmmo);

	void UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo);

protected:

	/* Hold array of inventory slots */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Children")
	TArray<class USOverlayInventorySlot*> InventorySlots;

	/* Hold manually updated copy of players ammo inventory */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Children")
	FAmmoInfo AmmoInventoryCopy;

	/* Keep track of currently active slot locally */
	class USOverlayInventorySlot* CurrentSlot;

};
