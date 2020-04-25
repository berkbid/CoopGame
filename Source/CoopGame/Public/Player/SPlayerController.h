// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


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

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitProperties() override;

	virtual void OnRep_PlayerState() override;

	/** spawns and initializes the PlayerState for this Controller */
	virtual void InitPlayerState() override;

	// Return success or failure for picking up weapon, based on inventory space
	bool PickedUpNewWeapon(const FWeaponInfo& WeaponInfo, bool bDidInteract);

	// Return extra ammo if ammo inventory doesn't have room for that type of ammo
	int32 PickedUpNewAmmo(EAmmoType AmmoType, int32 AmmoTotal);

	/**
	 * Called by server from owned character when trying to reload currently equipped weapon - returns 
	 * amount of ammo based on current clip size, max clip size, and available ammo 
	 */
	int32 ReloadAmmoClip();

	/** Called by weapons every time they fire so player controller can update HUD or clip data */
	void UpdateCurrentClip(int32 NewClipSize);

	/* Getter function for weapon pickups to display different information */
	bool GetIsInventoryFull() const;

	bool GetIsAmmoTypeFull(EAmmoType AmmoTypeToCheck) const;

	void HandlePawnDied();

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
	void ClientAddPlayerToHUDScoreboard(const FString& NewPlayerName, uint32 NewPlayerNumber);
	void ClientAddPlayerToHUDScoreboard_Implementation(const FString& NewPlayerName, uint32 NewPlayerNumber);

	/** Replicated function sent by server to client - called from weapon classes after firing */
	UFUNCTION(unreliable, client)
	void ClientUpdateClipHUD(int32 CurrentAmmo);
	void ClientUpdateClipHUD_Implementation(int32 CurrentAmmo);

protected:
	virtual void BeginPlay() override;

	// Helper method for tracing on Tick() method
	void TraceForInteractables();

	/* Helper method for both Tick() function and Interact() function to use for the same line trace parameters */
	class ASInteractable* FindTraceHitInteractable();

	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;

	virtual void OnUnPossess() override;

	/* Called by server - changes active inventory slot and tries to equip weapon if pawn exists */
	void EquipWeapon(uint8 NewWeaponSlot);

	////////////////////////////////////////
	// Player Input bindings
	////////////////////////////////////////
	// Input to interact with item based on line trace
	void Interact();

	/* Toggles between game play and interactable inventory UI */
	void ToggleInventory();

	// Inputs to change active inventory slot - doesn't require pawn
	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();
	void EquipSlotSix();

	////////////////////////////////////////////////////////////////////////////////
	// Replicated functions client to server - to handle player input authoritatively
	////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(Reliable, Server)
	void ServerInteract();
	void ServerInteract_Implementation();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerEquipWeaponSlot(uint8 SlotToEquip);
	void ServerEquipWeaponSlot_Implementation(uint8 SlotToEquip);
	bool ServerEquipWeaponSlot_Validate(uint8 SlotToEquip);

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
	void ClientPickupWeaponHUD(const FWeaponInfo& WeaponInfo, int32 SlotToUpdate);
	void ClientPickupWeaponHUD_Implementation(const FWeaponInfo& WeaponInfo, int32 SlotToUpdate);

	UFUNCTION(Reliable, Client)
	void ClientPickupAmmoHUD(EAmmoType NewAmmoType, int32 NewExtraAmmo);
	void ClientPickupAmmoHUD_Implementation(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	UFUNCTION(Reliable, Client)
	void ClientInitAmmoInventoryHUD(const FAmmoInfo& NewAmmoInfo);
	void ClientInitAmmoInventoryHUD_Implementation(const FAmmoInfo& NewAmmoInfo);

protected:
	/* Server controlled variable which the client also updates when passed the value in server to client RPC */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<FWeaponInfo> WeaponInventory;

	/* Server controlled variable which the client also updates when passed the value in server to client RPC */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	FAmmoInfo AmmoInventory;

	/* How far the line trace should travel for interacting with items */
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	float ItemTraceDistance;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	class USoundBase* PickupWeaponSound;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	class USoundBase* PickupAmmoSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class USUserWidgetGameInfo> wGameInfo;

	class USUserWidgetGameInfo* MyGameInfo;

	/* Client controlled pointer used in Tick() line trace to keep track of selected item */
	class ASInteractable* CurrentSelectedInteractable;

	FCollisionObjectQueryParams TraceObjectQueryParams;

	/* Keep track of which weapon slot is currently equipped, client also updates when passed the value in server to client RPC */
	UPROPERTY()
	uint8 CurrentSlot;

	/* Manually set property in the constructor, set to 6 because we only have keybinds and HUD setup for 6 slots */
	uint8 InventoryMaxSize;

	/* Server controlled variable which the client also updates when passed the value from server to client RPC */
	uint8 CurrentInventorySize;

	/* Server controlled variable which the client also updates when passed the value from server to client RPC */
	bool bIsInventoryFull;
};