// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventoryHUD.generated.h"


class UWidgetInventorySlot;
class UUserWidget;
class UWidgetAmmoDisplay;
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

	int32 GetExtraAmmoOfType(EAmmoType NewAmmoType);

	void UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	/* When we only need to update weapon info and not change to slot */
	void UpdateWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo);

	void SetMiniAmmoText(FString NewText);
	void SetMediumAmmoText(FString NewText);
	void SetHeavyAmmoText(FString NewText);
	void SetShellAmmoText(FString NewText);
	void SetRocketAmmoText(FString NewText);
	void HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo, class UTexture2D* WeaponTexture);

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
