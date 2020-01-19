// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* ImageBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* WeaponImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SlotAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* SlotAmmoImage;

	void UpdateAmmoText();

public:
	void InitSlot(class UTexture2D* WeaponTexture, const FWeaponInfo& NewWeaponInfo, const TMap<EAmmoType, class UTexture2D*>& AmmoTextureMap);

	void ActivateSlot();

	void ResetSlot();

	void UpdateCurrentAmmo(int32 NewCurrentAmmo);

	void UpdateExtraAmmo(int32 NewExtraAmmo);

	void UpdateBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo);

	// Weapon data for slot
	FWeaponInfo CurrentWeaponInfo;

	// Slot specific data
	int32 CurrentSlotAmmo;
	int32 CurrentExtraAmmo;
};
