// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetGameInfo.generated.h"

class UTextBlock;
class UBorder;
class ASWeapon;
class UTexture2D;

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetGameInfo : public UUserWidget
{
	GENERATED_BODY()

protected:

	/* Actor that widget is attached to via WidgetComponent */
	UPROPERTY(BlueprintReadOnly, Category = "ActorWidget")
	APlayerController* OwningController;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetInventoryHUD* InventoryHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetScoreboardHUD* ScoreboardHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetInventoryPage* InventoryInfoPage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* StateText;

	/* Mapping from Weapon class type to textures */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TMap<TSubclassOf<ASWeapon>, UTexture2D*> WeaponToTextureMap;

	/* Mapping from Ammo Types to textures */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoTextures")
	TMap<EAmmoType, UTexture2D*> AmmoToTextureMap;

public:

	virtual void SynchronizeProperties() override;

	virtual bool Initialize() override;

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "LODZERO|UI")
	void SetOwningController(class APlayerController* NewController);

	bool ToggleInventoryPage();

	void HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo);

	void HandlePickupAmmo(EAmmoType NewAmmoType, int32 NewAmmo);

	/* Handles reload situation with new ammo amounts also updating other slots of same ammo type*/
	void HandleReloadAmmoType(EAmmoType NewAmmoType, int32 CurrentAmmo, int32 ExtraAmmo);

	/* Handles updating current clip ammo */
	void UpdateCurrentClipAmmo(int32 NewCurrentAmmo);

	/* Handles inventory functionality to change to new slot with new ammo amounts */
	void InventoryChangeToSlot(int32 WeaponSlot);

	void InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo);

	void AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber);

	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);

	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);

	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);

	void SetStateText(FString NewState);
};
