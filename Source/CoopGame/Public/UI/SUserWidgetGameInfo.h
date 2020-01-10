// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetGameInfo.generated.h"

class UTextBlock;
class UBorder;
class USOverlayInventorySlot;
class USUserWidgetPlayerStats;
class ASWeapon;

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

	//you can make the binding optional with BindWidgetOptional instead. But don’t forget to null check!
/* Reference to HealthBar ProgressBar in Widget*/
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* ScoreboardEntryBox;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* StateText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MiniAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MediumAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HeavyAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ShellAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RocketAmmoText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UHorizontalBox* WeaponHUD;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* CurrentInventoryInfo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USHorizontalBoxInventory* InventoryContainer;


	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class USVerticalBoxCurrentWeapon* CurrentWeaponInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
	TSubclassOf<USUserWidgetPlayerStats> wPlayerStats;

	/* Mapping from unique PlayerID to their scoreboard PlayerStats object reference */
	TMap<uint32, USUserWidgetPlayerStats*> ScoreboardDictionary;

	/* Mapping from Weapon class type to textures */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TMap<TSubclassOf<ASWeapon>, class UTexture2D*> WeaponToTextureMap;

	FAmmoInfo CurrentAmmoInfo;

public:

	virtual bool Initialize() override;

	/* Set the owning actor so widgets have access to whatever is, broadcasting OnOwningActorChanged event */
	UFUNCTION(BlueprintCallable, Category = "LODZERO|UI")
	void SetOwningController(class APlayerController* NewController);

	void HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo);

	void HandlePickupAmmo(EAmmoType NewAmmoType, int32 NewAmmo);

	/* Handles reload situation with new ammo amounts also updating other slots of same ammo type*/
	void HandleReloadAmmoType(EAmmoType NewAmmoType, int32 CurrentAmmo, int32 ExtraAmmo);

	/* Handles updating current clip ammo */
	void UpdateCurrentClipAmmo(int32 NewCurrentAmmo);

	/* Handles inventory functionality to change to new slot with new ammo amounts */
	void InventoryChangeToSlot(int32 WeaponSlot);

	void InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo);

	/* When we only need to update weapon info and not change to slot */
	void UpdateWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo);

	class UVerticalBox* GiveInventoryWidget();

	void RestoreInventoryWidget();

	void AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber);

	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);

	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);

	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);

	void SetStateText(FString NewState);

	void SetMiniAmmoText(FString NewText);

	void SetMediumAmmoText(FString NewText);

	void SetHeavyAmmoText(FString NewText);

	void SetShellAmmoText(FString NewText);

	void SetRocketAmmoText(FString NewText);
	
};
