// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"


class ASWeapon;

UENUM(BlueprintType)		
enum class EAmmoType : uint8
{
	MiniAmmo,

	MediumAmmo,

	HeavyAmmo,

	ShellAmmo,

	RocketAmmo
};


UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common,

	Uncommon,

	Rare,

	Epic,

	Legendary

};

USTRUCT(BlueprintType)
struct FAmmoInfo
{
	GENERATED_BODY()



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MiniCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MediumCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 HeavyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ShellCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 RocketCount;

	// For Garbage Cleanup
	void Destroy()
	{

	}

	// Default constructor
	FAmmoInfo()
	{
		MiniCount = 0;
		MediumCount = 0;
		HeavyCount = 0;
		ShellCount = 0;
		RocketCount = 0;
	}

	
	FAmmoInfo(int32 NewMiniAmmo, int32 NewMediumAmmo, int32 NewHeavyAmmo, int32 NewShellAmmo, int32 NewRocketAmmo)
	{
		MiniCount = NewMiniAmmo;
		MediumCount = NewMediumAmmo;
		HeavyCount = NewHeavyAmmo;
		ShellCount = NewShellAmmo;
		RocketCount = NewRocketAmmo;
	}
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UClass* WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponRarity WeaponRarity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MaxAmmo;

	// For Garbage Cleanup
	void Destroy()
	{
		WeaponType = NULL;
	}

	// Default constructor
	FWeaponInfo()
	{
		WeaponType = NULL;
		WeaponRarity = EWeaponRarity::Common;
		AmmoType = EAmmoType::MiniAmmo;
		CurrentAmmo = 0;
		MaxAmmo = 0;
	}

	// Constructor with parameters for properties
	FWeaponInfo(UClass* NewWeaponClass, EWeaponRarity NewWeaponRarity, EAmmoType NewAmmoType, int32 NewCurrentAmmo, int32 NewMaxAmmo)
	{
		WeaponType = NewWeaponClass;
		WeaponRarity = NewWeaponRarity;
		AmmoType = NewAmmoType;
		CurrentAmmo = NewCurrentAmmo;
		MaxAmmo = NewMaxAmmo;
	}
};


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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void OnRep_PlayerState() override;

	/** spawns and initializes the PlayerState for this Controller */
	virtual void InitPlayerState() override;

	// Called from GameMode to allow server to run code and call ClientPostLogin()
	void ServerPostLogin();

	// Needs to be set Reliable, GameMode calls this OnPostLogin
	UFUNCTION(Client, Reliable)
	void ClientPostLogin();

	UFUNCTION(Client, Reliable)
	void ClientAddPlayerToHUDScoreboard(FString const &NewPlayerName, uint32 NewPlayerNumber);

	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);

	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);

	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);

	void SetStateText(FString NewState);

	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wGameInfo;

	class USUserWidgetGameInfo* MyGameInfo;

	// Return success or failure for picking up weapon, based on inventory space
	bool PickedUpNewWeapon(const FWeaponInfo& WeaponInfo);

	int32 GrabAmmoOfType(EAmmoType AmmoType, int32 CurrentClipSize, int32 MaxClipSize);

	/* Update current clip size for HUD */
	UFUNCTION(Client, Unreliable)
	void ClientUpdateClipHUD(int32 CurrentAmmo);

	bool bIsInventoryFull;

protected:
	virtual void SetupInputComponent() override;

	virtual void OnPossess(APawn* aPawn) override;

	void SetupInitialHUDState();

	void EquipWeapon(int NewWeaponSlot);

	void ChangeToSlotHUD(int32 NewSlot);

	UFUNCTION(Client, Reliable)
	void ClientChangeToSlotHUD(int32 NewSlot);

	UFUNCTION(Client, Reliable)
	void ClientHandleReloadHUD(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo);

	UFUNCTION(Client, Reliable)
	void ClientPickupWeaponHUD(FWeaponInfo WeaponInfo, int32 SlotToUpdate);

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

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Inventory")
	TArray<FWeaponInfo> WeaponInventory;

	UPROPERTY(Replicated, EditDefaultsOnly, Category = "Inventory")
	FAmmoInfo AmmoInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	class USoundBase* PickedupSound;

	/* Keep track of which weapon slot is currently equipped */
	UPROPERTY(Replicated)
	int CurrentSlot;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int InventoryMaxSize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int CurrentInventorySize;

};
