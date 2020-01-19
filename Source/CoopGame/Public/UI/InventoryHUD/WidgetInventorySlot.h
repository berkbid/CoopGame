// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventorySlot.generated.h"


class UTexture2D;


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
	void InitSlot(const FWeaponInfo& NewWeaponInfo, const TMap<TSubclassOf<class ASWeapon>, UTexture2D*>& WeaponTextureMapRef, const TMap<EAmmoType, UTexture2D*>& AmmoTextureMapRef);

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

	// Store textures for ammo and weapon so they can accessed and used on other parts of HUD
	UTexture2D* CurrentAmmoTexture;
	UTexture2D* CurrentWeaponTexture;
};
