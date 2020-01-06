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
	/** Default UObject constructor. */
	ASPlayerController(const FObjectInitializer& ObjectInitializer);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PostInitProperties() override;

	virtual void OnRep_PlayerState() override;

	/** spawns and initializes the PlayerState for this Controller */
	virtual void InitPlayerState() override;

	// Return success or failure for picking up weapon, based on inventory space
	bool PickedUpNewWeapon(const FWeaponInfo& WeaponInfo);

	// Return extra ammo if ammo inventory doesn't have room for that type of ammo
	int32 PickedUpNewAmmo(EAmmoType AmmoType, int32 AmmoAmount);

	/**
	 * Called by server from owned character when trying to reload currently equipped weapon - returns 
	 * amount of ammo based on current clip size, max clip size, and available ammo 
	 */
	int32 ReloadAmmoClip(int32 CurrentClipSize);

	///////////////////////////////////////////////////////////////////////////////////////////
	// Client called functions from OnRep variables on all PlayerStates, to update HUD scoreboard
	///////////////////////////////////////////////////////////////////////////////////////////

	void SetStateTextHUD(FString NewState);
	void UpdatePlayerScoreHUD(uint32 PlayerNumber, float NewScore);
	void UpdatePlayerKillsHUD(uint32 PlayerNumber, uint32 NewKills);
	void UpdatePlayerDeathsHUD(uint32 PlayerNumber, uint32 NewDeaths);

	// Called from GameMode to allow server to run code and call ClientPostLogin()
	void ServerPostLogin();

	/** Replicated function sent by server to client - called OnPostLogin */
	UFUNCTION(Reliable, Client)
	void ClientPostLogin();
	void ClientPostLogin_Implementation();

	/** Replicated function sent by server to client - called from game mode */
	UFUNCTION(Reliable, Client)
	void ClientAddPlayerToHUDScoreboard(FString const& NewPlayerName, uint32 NewPlayerNumber);
	void ClientAddPlayerToHUDScoreboard_Implementation(FString const& NewPlayerName, uint32 NewPlayerNumber);

	/** Replicated function sent by server to client - called from weapon classes after firing */
	UFUNCTION(unreliable, client)
	void ClientUpdateClipHUD(int32 CurrentAmmo);
	void ClientUpdateClipHUD_Implementation(int32 CurrentAmmo);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;

	/* Called by server - changes active inventory slot and tries to equip weapon if pawn exists */
	void EquipWeapon(int NewWeaponSlot);

	////////////////////////////////////////
	// Player Input bindings
	////////////////////////////////////////

	/* Toggles between game play and interactable inventory UI */
	void ToggleInventory();

	// Inputs to change active inventory slot - doesn't require pawn
	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();

	// Input to interact with item based on line trace
	void Interact();

	////////////////////////////////////////////////////////////////////////////////
	// Replicated functions client to server - to handle player input authoritatively
	////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(Reliable, Server)
	void ServerInteract();
	void ServerInteract_Implementation();

	UFUNCTION(Reliable, Server)
	void ServerEquipWeaponOne();
	void ServerEquipWeaponOne_Implementation();

	UFUNCTION(Reliable, Server)
	void ServerEquipWeaponTwo();
	void ServerEquipWeaponTwo_Implementation();

	UFUNCTION(Reliable, Server)
	void ServerEquipWeaponThree();
	void ServerEquipWeaponThree_Implementation();

	UFUNCTION(Reliable, Server)
	void ServerEquipWeaponFour();
	void ServerEquipWeaponFour_Implementation();

	UFUNCTION(Reliable, Server)
	void ServerEquipWeaponFive();
	void ServerEquipWeaponFive_Implementation();


	////////////////////////////////////////////////////////////
	//Replicated functions server to client - handles updating HUD data/display
	////////////////////////////////////////////////////////////

	UFUNCTION(Reliable, Client)
	void ClientChangeToSlotHUD(int32 NewSlot);
	void ClientChangeToSlotHUD_Implementation(int32 NewSlot);

	UFUNCTION(Reliable, Client)
	void ClientHandleReloadHUD(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo);
	void ClientHandleReloadHUD_Implementation(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo);

	UFUNCTION(Reliable, Client)
	void ClientPickupWeaponHUD(const FWeaponInfo& WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate);
	void ClientPickupWeaponHUD_Implementation(const FWeaponInfo& WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate);

	UFUNCTION(Reliable, Client)
	void ClientPickupAmmoHUD(EAmmoType NewAmmoType, int32 NewExtraAmmo);
	void ClientPickupAmmoHUD_Implementation(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	UFUNCTION(Reliable, Client)
	void ClientInitAmmoInventoryHUD(const FAmmoInfo &NewAmmoInfo);
	void ClientInitAmmoInventoryHUD_Implementation(const FAmmoInfo& NewAmmoInfo);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class USUserWidgetInventoryInfo> wInventoryInfo;

	class USUserWidgetGameInfo* MyGameInfo;

	class USUserWidgetInventoryInfo* MyInventoryInfo;

	/* Keep track of which weapon slot is currently equipped */
	UPROPERTY()
	int CurrentSlot;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int InventoryMaxSize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int CurrentInventorySize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	bool bIsInventoryFull;
};
