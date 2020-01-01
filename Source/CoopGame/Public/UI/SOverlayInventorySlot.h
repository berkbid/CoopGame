// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Components/Overlay.h"
#include "SOverlayInventorySlot.generated.h"


/**
 * 
 */
UCLASS()
class COOPGAME_API USOverlayInventorySlot : public UOverlay
{
	GENERATED_BODY()
	



public:
	USOverlayInventorySlot();

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/**
	 * Applies all properties to the native widget if possible.  This is called after a widget is constructed.
	 * It can also be called by the editor to update modified state, so ensure all initialization to a widgets
	 * properties are performed here, or the property and visual state may become unsynced.
	 */
	virtual void SynchronizeProperties() override;

	/**
	 * Called after the C++ constructor and after the properties have been initialized, including those loaded from config.
	 * This is called before any serialization or other setup has happened.
	 */
	virtual void PostInitProperties() override;

	void UpdateCurrentAmmo(int32 NewCurrentAmmo);

	void UpdateExtraAmmo(int32 NewExtraAmmo);

	void UpdateBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo);

	void ResetSlot();

	void ActivateSlot();

	void InitSlot(class UTexture2D* WeaponTexture, const FWeaponInfo& NewWeaponInfo, int32 ExtraAmmo);

	void UpdateAmmoText();

	// Weapon data for slot
	FWeaponInfo CurrentWeaponInfo;

	// Slot specific data
	int32 CurrentSlotAmmo;
	int32 CurrentExtraAmmo;


protected:



	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UBorder* SlotBorder;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UImage* SlotImage;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UTextBlock* AmmoText;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface
};
