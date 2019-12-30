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
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;

	// These are replicated variables that the server updates and owning client updates HUD OnRep
	CurrentSlot = 0;

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

// Listen server doesn't run this code
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
	ClientPostLogin();

}

// PlayerState isn't valid at this point for clients
// GameState is also not valid
void ASPlayerController::ClientPostLogin_Implementation()
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

		// Call HUD functions to setup initial HUD state
		SetupInitialHUDState();
	}
}

// client doing stuff with weaponinventory in here
// Self contained function in charge of setting initial values in different HUD objects
// PlayerState isn't valid here for clients, only for listen server
// GameState is only valid for server
void ASPlayerController::SetupInitialHUDState()
{
	if (!MyGameInfo) { return; }

	// Can't initialize game state here, do this at begin play
	// Loop through WeaponInventory array and update HUD images if weapons are present
	int32 WeaponInventoryLen = WeaponInventory.Num();

	for (int32 i = 0; i != WeaponInventoryLen; ++i)
	{
		if (WeaponInventory[i].WeaponType)
		{
			int32 NewMaxAmmo = 0;
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
			// Set initial HUD state for weapon slot
			MyGameInfo->HandlePickupWeapon(i, WeaponInventory[i].WeaponType, WeaponInventory[i].CurrentAmmo, NewMaxAmmo);
		}

		// Set inventory slot as active
		if (i == CurrentSlot)
		{
			//MyGameInfo->InventoryChangeToSlot(CurrentSlot, WeaponInventory[i].CurrentAmmo, );
			ChangeSlot(i);
		}
	}

	// Shouldn't have client reading from ammoinventory
	MyGameInfo->SetMiniAmmoText(FString::FromInt(AmmoInventory.MiniCount));
	MyGameInfo->SetMediumAmmoText(FString::FromInt(AmmoInventory.MediumCount));
	MyGameInfo->SetHeavyAmmoText(FString::FromInt(AmmoInventory.HeavyCount));
	MyGameInfo->SetShellAmmoText(FString::FromInt(AmmoInventory.ShellCount));
	MyGameInfo->SetRocketAmmoText(FString::FromInt(AmmoInventory.RocketCount));
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

//void ASPlayerController::SetSlotAmmo(int32 WeaponSlot, EAmmoType NewAmmoType, int32 NewAmmoAmount)
//{
//	if (!MyGameInfo) { return; }
//	
//	MyGameInfo->InventoryUpdateAmmo(WeaponSlot,  NewAmmoAmount);
//}

// Server should call this function
// Decrement AmmoType in our AmmoInventory an update HUD for clients
void ASPlayerController::DecrementAmmoType(EAmmoType AmmoType, int32 CurrentAmmo)
{
	switch (AmmoType)
	{
	case EAmmoType::MiniAmmo:
		if (AmmoInventory.MiniCount > 0)
		{
			AmmoInventory.MiniCount--;
		}
		ClientUpdateHudAmmo(AmmoType, CurrentAmmo, AmmoInventory.MiniCount);
		break;
	case EAmmoType::MediumAmmo:
		if (AmmoInventory.MediumCount > 0)
		{
			AmmoInventory.MediumCount--;
		}
		ClientUpdateHudAmmo(AmmoType, CurrentAmmo, AmmoInventory.MediumCount);
		break;
	case EAmmoType::HeavyAmmo:
		if (AmmoInventory.HeavyCount > 0)
		{
			AmmoInventory.HeavyCount--;
		}
		ClientUpdateHudAmmo(AmmoType, CurrentAmmo, AmmoInventory.HeavyCount);
		break;
	case EAmmoType::ShellAmmo:
		if (AmmoInventory.ShellCount > 0)
		{
			AmmoInventory.ShellCount--;
		}
		ClientUpdateHudAmmo(AmmoType, CurrentAmmo, AmmoInventory.ShellCount);
		break;
	case EAmmoType::RocketAmmo:
		if (AmmoInventory.RocketCount > 0)
		{
			AmmoInventory.RocketCount--;
		}
		ClientUpdateHudAmmo(AmmoType, CurrentAmmo, AmmoInventory.RocketCount);
		break;
	default:
		break;
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
			// If our current slot is a valid inventory index
			int32 WeaponInventoryLen = WeaponInventory.Num();
			if (WeaponInventoryLen > CurrentSlot)
			{
				// Equip whatever weapon is in the current slot if we are a SPlayerCharacter
				MySPlayerChar->EquipWeaponClass(WeaponInventory[CurrentSlot], CurrentSlot);
			}
		}
	}
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
	if (GetLocalRole() < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Should NOT call EquipWeapon as client! See SPlayerController.cpp"));
		return;
	}
	int OldSlot = CurrentSlot;
	// Replicated, Update current slot even if we have no weapon in that slot
	CurrentSlot = NewWeaponSlot;
	// If we are listen server, call function manually
	//if (IsLocalController()) { OnRep_SlotChange(); }
	

	TSubclassOf<ASWeapon> NewWeaponClass;
	// If array is long enough, equip whatever value is at that weapon index
	if (WeaponInventory.Num() > NewWeaponSlot) {

		NewWeaponClass = WeaponInventory[NewWeaponSlot].WeaponType;

		// Call ChangeWeapons, allowing for NewWeaponClass to be null going in, this will destroy current weapon
		// It is the same as changing to a new inventory slot that doesn't have a weapon in it
		ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
		if (MyPawn)
		{
			// Update Current Ammo of old weapon and equip new weapon
			int32 OldAmmoCount = MyPawn->EquipWeaponClass(WeaponInventory[NewWeaponSlot], NewWeaponSlot);

			// Store ammo count of weapon that was un-equipped in its FWeaponInfo struct in our WeaponInventory array
			if (OldAmmoCount >= 0)
			{
				WeaponInventory[OldSlot].CurrentAmmo = OldAmmoCount;
			}	
		}

		// Call client RPC to update HUD for newly equipped weapon
		ClientEquipWeaponHUD(CurrentSlot);
	}
}

// Server or client can run this as all data inside is replicated and being read only
void ASPlayerController::ChangeSlot(int32 NewSlot)
{
	if (MyGameInfo)
	{
		switch (WeaponInventory[NewSlot].AmmoType)
		{
		case EAmmoType::MiniAmmo:
			MyGameInfo->InventoryChangeToSlot(NewSlot, WeaponInventory[NewSlot].CurrentAmmo, AmmoInventory.MiniCount);
			break;
		case EAmmoType::MediumAmmo:
			MyGameInfo->InventoryChangeToSlot(NewSlot, WeaponInventory[NewSlot].CurrentAmmo, AmmoInventory.MediumCount);
			break;
		case EAmmoType::HeavyAmmo:
			MyGameInfo->InventoryChangeToSlot(NewSlot, WeaponInventory[NewSlot].CurrentAmmo, AmmoInventory.HeavyCount);
			break;
		case EAmmoType::ShellAmmo:
			MyGameInfo->InventoryChangeToSlot(NewSlot, WeaponInventory[NewSlot].CurrentAmmo, AmmoInventory.ShellCount);
			break;
		case EAmmoType::RocketAmmo:
			MyGameInfo->InventoryChangeToSlot(NewSlot, WeaponInventory[NewSlot].CurrentAmmo, AmmoInventory.RocketCount);
			break;
		default:
			break;
		}
	}
}

void ASPlayerController::ClientEquipWeaponHUD_Implementation(int32 NewSlot)
{
	ChangeSlot(NewSlot);
}

// Server gives client necessary info to update HUD every time player shoots
void ASPlayerController::ClientUpdateHudAmmo_Implementation(EAmmoType AmmoType, int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!MyGameInfo) { return; }

	// Set general ammo text
	MyGameInfo->SetWeaponAmmo(CurrentAmmo, MaxAmmo);

	// Set slot ammo text, need to add one to current ammo
	MyGameInfo->InventoryUpdateAmmo(CurrentSlot, CurrentAmmo, MaxAmmo);

	switch (AmmoType)
	{
	case EAmmoType::MiniAmmo:
		MyGameInfo->SetMiniAmmoText(FString::FromInt(MaxAmmo));
		break;
	case EAmmoType::MediumAmmo:
		MyGameInfo->SetMediumAmmoText(FString::FromInt(MaxAmmo));
		break;
	case EAmmoType::HeavyAmmo:
		MyGameInfo->SetHeavyAmmoText(FString::FromInt(MaxAmmo));
		break;
	case EAmmoType::ShellAmmo:
		MyGameInfo->SetShellAmmoText(FString::FromInt(MaxAmmo));
		break;
	case EAmmoType::RocketAmmo:
		MyGameInfo->SetRocketAmmoText(FString::FromInt(MaxAmmo));
		break;
	default:
		break;
	}
}

// When server PICKS UP NEW WEAPON, updates SlotToUpdate so owning client updates HUD for that slot
// This update for HUD is equivalent to equipping and un-equipping the weapon, show weapon image or remove it
void ASPlayerController::ClientPickupWeaponHUD_Implementation(FWeaponInfo WeaponInfo, int32 SlotToUpdate)
{
	// Handle HUD for picking up new weapon
	if (MyGameInfo)
	{
		int32 NewMaxAmmo = 0;
		switch (WeaponInfo.AmmoType)
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
		// Set initial HUD state for weapon s lot, including picture and ammo amount
		MyGameInfo->HandlePickupWeapon(SlotToUpdate, WeaponInfo.WeaponType, WeaponInfo.CurrentAmmo, NewMaxAmmo);
		
	}

	// Only play weapon pickup sound if we aren't already selecting on new weapon slot, this is because weapon swap sound will play
	// We don't want double sound. This works fine as long as weapon pickup sound is same as weapon swap sound.
	// Instead of calling this code here, call a function on the pawn that gets the sound pointer from the actual weapon to play sound on pickups.
	if (CurrentSlot != SlotToUpdate)
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
	// If we did pickup a weapon in our current slot
	else
	{
		
	}
}

// Try to pick up weapon if inventory has space, return success or failure
bool ASPlayerController::PickedUpNewWeapon(FWeaponInfo WeaponInfo)
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
		
			// Update HUD elements for new weapon
			ClientPickupWeaponHUD(WeaponInfo, i);
			// want to inventoryupdateammo

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
int32 ASPlayerController::GrabAmmoOfType(EAmmoType AmmoType, int32 CurrentAmmo, int32 MaxAmmo)
{
	int32 AmmmoNeeded = MaxAmmo - CurrentAmmo;
	if (AmmmoNeeded <= 0) { return 0; }

	int32 AmmoReturnAmount = 0;
	int32 TempMaxAmmo = 0;

	switch (AmmoType)
	{
	case EAmmoType::MiniAmmo:
		TempMaxAmmo = AmmoInventory.MiniCount;

		if (TempMaxAmmo > 0)
		{
			AmmoReturnAmount = FMath::Min(AmmmoNeeded, TempMaxAmmo);
			AmmoInventory.MiniCount -= AmmoReturnAmount;
			TempMaxAmmo = AmmoInventory.MiniCount;
		}
		break;
	case EAmmoType::MediumAmmo:
		TempMaxAmmo = AmmoInventory.MediumCount;

		if (TempMaxAmmo > 0)
		{
			AmmoReturnAmount = FMath::Min(AmmmoNeeded, TempMaxAmmo);
			AmmoInventory.MediumCount -= AmmoReturnAmount;
			TempMaxAmmo = AmmoInventory.MediumCount;
		}
		break;
	case EAmmoType::HeavyAmmo:
		TempMaxAmmo = AmmoInventory.HeavyCount;

		if (TempMaxAmmo > 0)
		{
			AmmoReturnAmount = FMath::Min(AmmmoNeeded, TempMaxAmmo);
			AmmoInventory.HeavyCount -= AmmoReturnAmount;
			TempMaxAmmo = AmmoInventory.HeavyCount;
		}
		break;
	case EAmmoType::ShellAmmo:
		TempMaxAmmo = AmmoInventory.ShellCount;

		if (TempMaxAmmo > 0)
		{
			AmmoReturnAmount = FMath::Min(AmmmoNeeded, TempMaxAmmo);
			AmmoInventory.ShellCount -= AmmoReturnAmount;
			TempMaxAmmo = AmmoInventory.ShellCount;
		}
		break;
	case EAmmoType::RocketAmmo:
		TempMaxAmmo = AmmoInventory.RocketCount;

		if (TempMaxAmmo > 0)
		{
			AmmoReturnAmount = FMath::Min(AmmmoNeeded, TempMaxAmmo);
			AmmoInventory.RocketCount -= AmmoReturnAmount;
			TempMaxAmmo = AmmoInventory.RocketCount;
		}
		break;
	default:
		break;
	}

	//need to update ammo slot HUD for all slots using same ammo type
	ClientUpdateHudAmmo(AmmoType, CurrentAmmo + AmmoReturnAmount, TempMaxAmmo);
	return AmmoReturnAmount;
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// WeaponInventory is an array of the weapons the controller has, server updates this array and owning client needs the data
	DOREPLIFETIME_CONDITION(ASPlayerController, WeaponInventory, COND_OwnerOnly);

	DOREPLIFETIME_CONDITION(ASPlayerController, AmmoInventory, COND_OwnerOnly);

	// Server updates this property and owning client reacts by updating HUD showing newly "selected" inventory slot
	DOREPLIFETIME_CONDITION(ASPlayerController, CurrentSlot, COND_OwnerOnly);

}