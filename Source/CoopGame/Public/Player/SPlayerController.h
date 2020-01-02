// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


class ASWeapon;



/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()


public:
	
	ASPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitProperties() override;

	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;

	/** spawns and initializes the PlayerState for this Controller */
	virtual void InitPlayerState() override;

	// Called from GameMode to allow server to run code and call ClientPostLogin()
	void ServerPostLogin();

	// Needs to be set Reliable, GameMode calls this OnPostLogin
	UFUNCTION(Client, Reliable)
	void ClientPostLogin();

	// Return success or failure for picking up weapon, based on inventory space
	bool PickedUpNewWeapon(const FWeaponInfo& WeaponInfo);

	// Return success or failure for picking up ammo, based on inventory space
	int32 PickedUpNewAmmo(EAmmoType AmmoType, int32 AmmoAmount);

	int32 ReloadAmmoClip(int32 CurrentClipSize, int32 MaxClipSize);

	/* For character to query player controller if inventory is full upon weapon pickup (getter for protected variable) */
	bool GetIsInventoryFull();

	void SetStateText(FString NewState);

	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);

	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);

	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);

	UFUNCTION(Client, Reliable)
	void ClientAddPlayerToHUDScoreboard(FString const& NewPlayerName, uint32 NewPlayerNumber);
	/* Update current clip size for HUD, called by weapons after firing */

	UFUNCTION(Client, Unreliable)
	void ClientUpdateClipHUD(int32 CurrentAmmo);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<FWeaponInfo> WeaponInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FAmmoInfo AmmoInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	class USoundBase* PickupWeaponSound;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	class USoundBase* PickupAmmoSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameInfo;

	class USUserWidgetGameInfo* MyGameInfo;

	/* Keep track of which weapon slot is currently equipped */
	UPROPERTY()
	int CurrentSlot;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int InventoryMaxSize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int CurrentInventorySize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	bool bIsInventoryFull;

protected:
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;

	void EquipWeapon(int NewWeaponSlot);

	int32 GetExtraAmmoOfType(EAmmoType QueryAmmoType);

	void ChangeToSlotHUD(int32 NewSlot);

	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponOne();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponTwo();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponThree();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponFour();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponFive();

	UFUNCTION(Client, Reliable)
	void ClientChangeToSlotHUD(int32 NewSlot);

	UFUNCTION(Client, Reliable)
	void ClientHandleReloadHUD(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo);

	UFUNCTION(Client, Reliable)
	void ClientPickupWeaponHUD(const FWeaponInfo& WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate);

	UFUNCTION(Client, Reliable)
	void ClientPickupAmmoHUD(EAmmoType NewAmmoType, int32 NewExtraAmmo);

};
