// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "SWeapon.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentSlot = 0;
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;

	// Setup initial WeaponInventory with appropriate size and NULL values
	WeaponInventory.Init(FWeaponInfo(), InventoryMaxSize);
	AmmoInventory = FAmmoInfo(50, 60, 20, 25, 12);
}

void ASPlayerController::PostInitProperties()
{
	Super::PostInitProperties();

	// Count current inventory size
	for (int32 i = 0; i != WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i].WeaponType)
		{
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}
		}
	}
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Update some HUD info manually here for clients since they might load in late
	if (MyGameInfo)
	{
		ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
		if (GS)
		{
			MyGameInfo->SetStateText(GS->GetWaveStateString());
		}
	}
}

// This gets replicated when PlayerState is assigned to this controller and is valid for the first time
// GameState isn't always valid at this point
void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}

/** spawns and initializes the PlayerState for this Controller */
// Called before possess and postlogin, only server runs this code
void ASPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

// Only PlayerState for server is valid in GameState
void ASPlayerController::ServerPostLogin()
{
	// Can run server code here if needed for RPC's
	ClientPostLogin(AmmoInventory);

	// Loop through WeaponInventory array and update HUD images if weapons are present
	int32 WeaponInventoryLen = WeaponInventory.Num();

	// Loop through our weapon inventory and populate HUD with the data
	for (int32 i = 0; i != WeaponInventoryLen; ++i)
	{
		int32 NewMaxAmmo = 0;
		if (WeaponInventory[i].WeaponType)
		{
			switch (WeaponInventory[i].AmmoType)
			{
			case EAmmoType::MiniAmmo:
				NewMaxAmmo = AmmoInventory.MiniCount;
				break;
			case EAmmoType::MediumAmmo:
				NewMaxAmmo = AmmoInventory.MediumCount;
				break;
			case EAmmoType::HeavyAmmo:
				NewMaxAmmo = AmmoInventory.HeavyCount;
				break;
			case EAmmoType::ShellAmmo:
				NewMaxAmmo = AmmoInventory.ShellCount;
				break;
			case EAmmoType::RocketAmmo:
				NewMaxAmmo = AmmoInventory.RocketCount;
				break;
			default:
				break;
			}
			// Handle HUD for picking up weapon, will detect if is in current slot by arguments
			ClientPickupWeaponHUD(WeaponInventory[i], CurrentSlot, i, NewMaxAmmo);
			if (i == CurrentSlot)
			{
				ClientChangeToSlotHUD(i);
			}
		}
	}
}

// Playerstate and gamestate not valid for clients here
void ASPlayerController::ClientPostLogin_Implementation(const FAmmoInfo& TempAmmoInfo)
{
	// Create and setup initial HUD state
	// Only local controllers can add widgets
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (!wGameInfo) { return; }
		MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);
		if (!MyGameInfo) { return; }

		// Pass reference of ourself to widget while calling setup logic on widget
		MyGameInfo->SetOwningController(this);
		MyGameInfo->AddToViewport();

		MyGameInfo->SetMiniAmmoText(FString::FromInt(TempAmmoInfo.MiniCount));
		MyGameInfo->SetMediumAmmoText(FString::FromInt(TempAmmoInfo.MediumCount));
		MyGameInfo->SetHeavyAmmoText(FString::FromInt(TempAmmoInfo.HeavyCount));
		MyGameInfo->SetShellAmmoText(FString::FromInt(TempAmmoInfo.ShellCount));
		MyGameInfo->SetRocketAmmoText(FString::FromInt(TempAmmoInfo.RocketCount));
	}
}

void ASPlayerController::ClientAddPlayerToHUDScoreboard_Implementation(FString const& NewPlayerName, uint32 NewPlayerNumber)
{
	if (MyGameInfo)
	{
		MyGameInfo->AddPlayerToScoreboard(NewPlayerName, NewPlayerNumber);
	}
}

void ASPlayerController::SetStateText(FString NewState)
{
	if (MyGameInfo)
	{
		MyGameInfo->SetStateText(NewState);
	}
}

void ASPlayerController::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerScore(PlayerNumber, NewScore);
	}
}

void ASPlayerController::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerKills(PlayerNumber, NewKills);
	}
}

void ASPlayerController::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerDeaths(PlayerNumber, NewDeaths);
	}
}

void ASPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// PlayerState is valid at this point, but playername is not set, so PostLogin hasent been called
	if (GetLocalRole() == ROLE_Authority)
	{
		// Uncomment to have players reset to current slot being 0 upon possessing new pawn
		//CurrentSlot = 0;
		//OnRep_SlotChange();

		// If we possess a SPlayerCharacter, equip weapon at CurrentSlot index
		ASPlayerCharacter* MySPlayerChar = Cast<ASPlayerCharacter>(GetPawn());
		if (MySPlayerChar)
		{
			// If our current slot is a valid inventory index, have player equip contents of inventory index
			int32 WeaponInventoryLen = WeaponInventory.Num();
			if (WeaponInventoryLen > CurrentSlot)
			{
				// Equip whatever weapon is in the current slot if we are a SPlayerCharacter
				MySPlayerChar->EquipWeaponClass(WeaponInventory[CurrentSlot], CurrentSlot);
			}
		}
	}
}

int32 ASPlayerController::GetExtraAmmoOfType(EAmmoType QueryAmmoType)
{
	int32 ReturnAmount = 0;

	switch (QueryAmmoType)
	{
	case EAmmoType::MiniAmmo:
		ReturnAmount = AmmoInventory.MiniCount;
		break;
	case EAmmoType::MediumAmmo:
		ReturnAmount = AmmoInventory.MediumCount;
		break;
	case EAmmoType::HeavyAmmo:
		ReturnAmount = AmmoInventory.HeavyCount;
		break;
	case EAmmoType::ShellAmmo:
		ReturnAmount = AmmoInventory.ShellCount;
		break;
	case EAmmoType::RocketAmmo:
		ReturnAmount = AmmoInventory.RocketCount;
		break;
	default:
		break;
	}

	return ReturnAmount;
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//check(InputComponent);
	// PlayerController is in charge of weapon changing, this can happen without a pawn, pawn uses the weapons
	InputComponent->BindAction("Weapon1", IE_Pressed, this, &ASPlayerController::EquipSlotOne);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &ASPlayerController::EquipSlotTwo);
	InputComponent->BindAction("Weapon3", IE_Pressed, this, &ASPlayerController::EquipSlotThree);
	InputComponent->BindAction("Weapon4", IE_Pressed, this, &ASPlayerController::EquipSlotFour);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);
}

void ASPlayerController::EquipSlotOne()
{
	ServerEquipWeaponOne();
}

void ASPlayerController::EquipSlotTwo()
{
	ServerEquipWeaponTwo();
}

void ASPlayerController::EquipSlotThree()
{
	ServerEquipWeaponThree();
}

void ASPlayerController::EquipSlotFour()
{
	ServerEquipWeaponFour();
}

void ASPlayerController::EquipSlotFive()
{
	ServerEquipWeaponFive();
}

void ASPlayerController::ServerEquipWeaponOne_Implementation()
{
	if (CurrentSlot == 0) { return; }
	EquipWeapon(0);
}

void ASPlayerController::ServerEquipWeaponTwo_Implementation()
{
	if (CurrentSlot == 1) { return; }
	EquipWeapon(1);
}

void ASPlayerController::ServerEquipWeaponThree_Implementation()
{
	if (CurrentSlot == 2) { return; }
	EquipWeapon(2);
}

void ASPlayerController::ServerEquipWeaponFour_Implementation()
{
	if (CurrentSlot == 3) { return; }
	EquipWeapon(3);
}

void ASPlayerController::ServerEquipWeaponFive_Implementation()
{
	if (CurrentSlot == 4) { return; }
	EquipWeapon(4);
}

// Method that handles equipping a new weapon slot of the inventory, only server should ever call this function
void ASPlayerController::EquipWeapon(int NewWeaponSlot)
{
	// Only server should call this function, this is precautionary
	if (GetLocalRole() < ROLE_Authority) { return; }

	// Store old slot to update weapon inventory on ammo count of old weapon when we receive this data
	int OldSlot = CurrentSlot;
	CurrentSlot = NewWeaponSlot;

	// Equip new weapon slot even if it is empty and update HUD
	if (WeaponInventory.Num() > NewWeaponSlot) 
	{
		// Call ChangeWeapons, allowing for NewWeaponClass to be null going in, this will destroy current weapon
		// It is the same as changing to a new inventory slot that doesn't have a weapon in it
		ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
		if (MyPawn)
		{
			// When we weapon swap, need to keep track of clip size of old weapon in our weapon inventory
			int32 OldAmmoCount = MyPawn->EquipWeaponClass(WeaponInventory[NewWeaponSlot], NewWeaponSlot);

			// Store ammo count of weapon that was un-equipped in its FWeaponInfo struct in our WeaponInventory array
			if (OldAmmoCount >= 0 && WeaponInventory.Num() > OldSlot)
			{
				WeaponInventory[OldSlot].CurrentAmmo = OldAmmoCount;
			}	
		}
		// Call client RPC to update HUD for newly equipped weapon
		ClientChangeToSlotHUD(CurrentSlot);
	}
}

// Client only should run this method when they want to update HUD for changing inventory slot
void ASPlayerController::ChangeToSlotHUD(int32 NewSlot)
{
	if (!MyGameInfo) { return; }

	// Call HUD method to change slot with ammo info
	MyGameInfo->InventoryChangeToSlot(NewSlot);
}

void ASPlayerController::ClientChangeToSlotHUD_Implementation(int32 NewSlot)
{
	ChangeToSlotHUD(NewSlot);
}

void ASPlayerController::ClientHandleReloadHUD_Implementation(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo)
{
	if (!MyGameInfo) { return; }

	MyGameInfo->HandleReloadAmmoType(NewAmmoType, NewClipAmmo, NewExtraAmmo);

}

// This update for HUD is equivalent to equipping and un-equipping the weapon, show weapon image or remove it
void ASPlayerController::ClientPickupWeaponHUD_Implementation(const FWeaponInfo &WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate, int32 ExtraAmmoAmount)
{
	// Handle HUD for picking up new weapon
	if (!MyGameInfo) { return; }
	
	// Set initial HUD state for weapon slot, including picture and ammo amount
	MyGameInfo->HandlePickupWeapon(SlotToUpdate, WeaponInfo, ExtraAmmoAmount);
		
	// Only play weapon pickup sound if we aren't already selecting on new weapon slot, this is because weapon swap sound will play
	if (TempCurrentSlot != SlotToUpdate)
	{
		// Play pickup sound also
		if (PickedupSound)
		{
			APawn* ControlledPawn = GetPawn();
			if (ControlledPawn)
			{
				UGameplayStatics::PlaySoundAtLocation(this, PickedupSound, ControlledPawn->GetActorLocation());
			}
		}
	}
	// If we did pickup a weapon in our current slot, update HUD properly
	else
	{
		// Call HUD method to change to slot
		MyGameInfo->UpdateWeaponInfo(WeaponInfo, ExtraAmmoAmount);
	}
}

// Try to pick up weapon if inventory has space, return success or failure
bool ASPlayerController::PickedUpNewWeapon(const FWeaponInfo &WeaponInfo)
{
	// Only server should call this function
	if (GetLocalRole() < ROLE_Authority) { return false; }
	
	// WeaponClass will never be null in here, it is checked in the previous function call
	// Loop through inventory looking for empty slot
	int32 InventorySize = WeaponInventory.Num();
	for (int32 i = 0; i < InventorySize; ++i)
	{
		// If we find an empty slot, give new weapon type and update HUD image for slot
		if (!WeaponInventory[i].WeaponType)
		{
			// Update weapon inventory slot to new weaponclass
			WeaponInventory[i] = WeaponInfo;
		
			int32 ExtraAmmoAmount = GetExtraAmmoOfType(WeaponInfo.AmmoType);
			// Update HUD elements for new weapon, also pass extra ammo if this weapon's ammo type
			ClientPickupWeaponHUD(WeaponInfo, CurrentSlot, i, ExtraAmmoAmount);

			// Update inventory size variable and set bIsInventoryFull
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}

			// If we pick up a weapon into our current selected EMPTY slot, equip the weapon!
			if (CurrentSlot == i)
			{
				ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
				if (MyPawn)
				{
					// This returns ammo amount of old weapon but we don't need it in this case
					MyPawn->EquipWeaponClass(WeaponInfo, i);			
				}
			}
			else
			{
				// Only want to play pickup sound in this scenario, or don't want to play weapon swap sound in "if" clause above
			}
			//We officially equipped new weapon, return success
			return true;
		}
	}
	// Return failure because we didn't find empty slot for weapon
	return false;
}

// We are server in here
int32 ASPlayerController::GrabAmmoOfType(int32 CurrentClipSize, int32 MaxClipSize)
{
	int32 AmmmoNeeded = MaxClipSize - CurrentClipSize;
	if (AmmmoNeeded <= 0) { return 0; }

	// Retrieve ammo from ammo inventory
	int32 AmmoReturnAmount = 0;
	int32 ExtraAmmoTemp = 0;
	EAmmoType AmmoTypeNeeded = WeaponInventory[CurrentSlot].AmmoType;
	AmmoInventory.RequestAmmo(AmmoTypeNeeded, AmmmoNeeded, AmmoReturnAmount, ExtraAmmoTemp);

	// Update HUD with new ammo inventory change
	ClientHandleReloadHUD(AmmoTypeNeeded, CurrentClipSize + AmmoReturnAmount, ExtraAmmoTemp);

	return AmmoReturnAmount;
}

void ASPlayerController::ClientUpdateClipHUD_Implementation(int32 CurrentAmmo)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdateCurrentClipAmmo(CurrentAmmo);
	}
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}