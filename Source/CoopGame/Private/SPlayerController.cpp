// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWeapon.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameMode.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;

	// These are replicated variables that the server updates and owning client updates HUD OnRep
	CurrentSlot = 0;
	SlotToUpdate = -1;

	// Setup initial WeaponInventory with appropriate size and NULL values
	WeaponInventory.Init(NULL, InventoryMaxSize);
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
			FString CurrentWaveStateString = GS->GetWaveStateString();
			MyGameInfo->SetStateText(CurrentWaveStateString);
		}

		//ASPlayerState* PS = GetPlayerState<ASPlayerState>();
		//if (PS)
		//{
		//	
		//}
	}
}


// Listen server doesn't run this code
// This gets replicated when PlayerState is assigned to this controller and is valid for the first time
void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AddPlayerToHUDScoreboard();

}

/** spawns and initializes the PlayerState for this Controller */
void ASPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

void ASPlayerController::AddPlayerToHUDScoreboard()
{
	// @TODO Listen server isn't running this code, need to fix
	// Add self stats to scoreboard now that our playerstate is valid to access
	ASPlayerState* PS = Cast<ASPlayerState>(PlayerState);
	if (PS && MyGameInfo)
	{
		MyGameInfo->AddPlayerToScoreboard(PS->GetPlayerName(), FString::FromInt(PS->PlayerKills), FString::FromInt(PS->PlayerDeaths), FString::SanitizeFloat(PS->Score));
	}
}

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

// Self contained function in charge of setting initial values in different HUD objects
void ASPlayerController::SetupInitialHUDState()
{
	if (!MyGameInfo) { return; }

	// Set starting score value to 0
	MyGameInfo->HandleScoreChanged(0.f);

	// Can't initialize game state here, do this at begin play
	// Loop through WeaponInventory array and update HUD images if weapons are present
	for (int32 i = 0; i != WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i])
		{
			// Must set color for selected inventory slot here because first OnPossess happens before BeginPlay
			if (i == CurrentSlot)
			{
				MyGameInfo->UpdateInventoryHUD(CurrentSlot);
			}

			MyGameInfo->SetInventoryImage(WeaponInventory[i], i);

			// Update current inventory size, need to do this on server since it is used on server
			CurrentInventorySize++;

			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}
		}
	}
}

void ASPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	
	if (Role == ROLE_Authority)
	{
		// Uncomment to have players reset to current slot being 0 upon possessing new pawn
		//CurrentSlot = 0;
		//OnRep_SlotChange();
		
		// If our current slot is a valid inventory index
		if (WeaponInventory.Num() > CurrentSlot)
		{
			// Try cast the pawn to a SCharacter
			ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());

			// Equip whatever weapon is in the current slot if we are a SCharacter
			if (MyPawn)
			{
				MyPawn->ChangeWeapons(WeaponInventory[CurrentSlot], CurrentSlot);
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
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Should NOT call EquipWeapon as client! See SPlayerController.cpp"));
		return;
	}

	// Replicated, Update current slot even if we have no weapon in that slot
	CurrentSlot = NewWeaponSlot;
	// If we are listen server, call function manually
	if (IsLocalController()) { OnRep_SlotChange(); }

	TSubclassOf<ASWeapon> NewWeaponClass;

	// If array is long enough, try to set NewWeaponClass to the value at its index
	if (WeaponInventory.Num() > NewWeaponSlot) {

		NewWeaponClass = WeaponInventory[NewWeaponSlot];
	}

	// Call ChangeWeapons, allowing for NewWeaponClass to be null going in, this will destroy current weapon
	// It is the same as changing to a new inventory slot that doesn't have a weapon in it
	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	if (MyPawn)
	{
		MyPawn->ChangeWeapons(NewWeaponClass, NewWeaponSlot);
	}
}

// Only server should be inside this call
bool ASPlayerController::PickedUpNewWeapon(TSubclassOf<ASWeapon> WeaponClass)
{
	// Only server should call this function, this is precautionary
	if (Role < ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Should NOT call PickedUpNewWeapon as client! See SPlayerController.cpp"));
		return false;
	}
	// Weaponclass will never be null in here, it is checked in the previous function call
	// Loop through inventory looking for empty slot
	for (int32 i = 0; i != WeaponInventory.Num(); ++i)
	{
		// If we find an empty slot, give new weapon type and update HUD image for slot
		if (WeaponInventory[i] == NULL)
		{
			// Update weaponinventory slot to new weaponclass
			WeaponInventory[i] = WeaponClass;
			CurrentInventorySize++;

			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}

			// Update HUD image for client/listen server
			SlotToUpdate = i;
			// If we are listen server, call function manually
			if (IsLocalController()) { OnRep_SlotToUpdate(); }
			
			// If we pick up a weapon into a new inventory slot, AND we are selected on that item slot, then try to equip weapon!!
			if (CurrentSlot == i)
			{
				ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
				if (MyPawn)
				{
					MyPawn->ChangeWeapons(WeaponClass, i);
				}
			}
			//We officially equipped new weapon, return success
			return true;
		}
	}
	// Return failure because we didn't find empty slot for weapon
	return false;
}

// When server updates CurrentSlot property, owning client updates their HUD
void ASPlayerController::OnRep_SlotChange()
{
	if (MyGameInfo) 
	{ 
		MyGameInfo->UpdateInventoryHUD(CurrentSlot);
	}
}

// When server picks up new weapon, updates SlotToUpdate so owning client updates HUD for that slot
// This update for HUD is equivalent to equipping and un-equipping the weapon, show weapon image or remove it
void ASPlayerController::OnRep_SlotToUpdate()
{
	if (MyGameInfo)
	{
		if (WeaponInventory.Num() > SlotToUpdate)
		{
			MyGameInfo->SetInventoryImage(WeaponInventory[SlotToUpdate], SlotToUpdate);
		}
	}
}

void ASPlayerController::SetScoreText(float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->HandleScoreChanged(NewScore);
	}
}

void ASPlayerController::SetStateText(FString NewState)
{
	if (IsLocalController())
	{
		if (MyGameInfo)
		{
			MyGameInfo->SetStateText(NewState);
		}
	}
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// WeaponInventory is an array of the weapons the controller has, server updates this array and owning client needs the data
	DOREPLIFETIME_CONDITION(ASPlayerController, WeaponInventory, COND_OwnerOnly);

	// Server updates this property and owning client reacts by updating HUD showing newly "selected" inventory slot
	DOREPLIFETIME_CONDITION(ASPlayerController, CurrentSlot, COND_OwnerOnly);

	// Server updates this property and owning client reacts by updating HUD showing newly "acquired" inventory slot
	DOREPLIFETIME_CONDITION(ASPlayerController, SlotToUpdate, COND_OwnerOnly);
}