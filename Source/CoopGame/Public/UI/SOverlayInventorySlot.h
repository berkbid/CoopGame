// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	void ResetSlot();

	void ActivateSlot();

	void InitSlot(class UTexture2D* WeaponTexture);

	void UpdateAmmoText(uint32 AmmoAmount);


protected:

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UTextBlock* AmmoText;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Children")
	class UBorder* SlotBorder;

	// UWidget interface
	virtual TSharedRef<SWidget> RebuildWidget() override;
	// End of UWidget interface

};
