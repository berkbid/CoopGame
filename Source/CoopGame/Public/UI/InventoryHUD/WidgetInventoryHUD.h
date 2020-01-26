// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventoryHUD.generated.h"


class UWidgetInventorySlot;
class UUserWidget;
class UWidgetAmmoDisplay;
class UTexture2D;

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetInventoryHUD : public UUserWidget
{
	GENERATED_BODY()
	


public:
	virtual bool Initialize() override;

	virtual void SynchronizeProperties() override;

	void HandleSlotChange(int32 WeaponSlot);

	void UpdateCurrentSlotAmmo(int32 CurrentAmmo);

	void InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo);

	void UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	/* Update weapon info display, data passed from data stored in slot */
	void UpdateWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo, UTexture2D* NewAmmoTexture);

	void InitInventoryHUD();

	/* Return reference to array of inventory slots */
	const TArray<UWidgetInventorySlot*>& GetInventorySlotWidgets();

	void SetMiniAmmoText(FString NewText);
	void SetMediumAmmoText(FString NewText);
	void SetHeavyAmmoText(FString NewText);
	void SetShellAmmoText(FString NewText);
	void SetRocketAmmoText(FString NewText);
	void HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo, const TMap<TSubclassOf<class ASWeapon>, UTexture2D*>& WeaponTextureMapRef, const TMap<EAmmoType, UTexture2D*>& AmmoTextureMapRef);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UWidgetInventorySlot*> InventorySlots;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* SlotGrid;

	// Bind ammo slot widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetWeaponDisplay* WeaponDisplay;

	// Bind ammo slot widgets
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetAmmoDisplay* MiniText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetAmmoDisplay* MediumText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetAmmoDisplay* HeavyText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetAmmoDisplay* ShellText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UWidgetAmmoDisplay* RocketText;

protected:
	/* Keep track of currently active slot locally */
	UWidgetInventorySlot* CurrentSlot;

	FAmmoInfo CurrentAmmoInfo;

};
