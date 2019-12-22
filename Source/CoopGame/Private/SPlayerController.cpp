// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWeapon.h"
#include "SPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

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
	}
}

// Listen server doesn't run this code
// This gets replicated when PlayerState is assigned to this controller and is valid for the first time
// GameState isn't always valid at this point
void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// This works for client and not listen server
	AddPlayerToHUDScoreboard(PlayerState);	

	//// Loop through all playerstates and add them to scoreboard
	//ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
	//if (GS)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("FOUND GameState"));
	//	TArray<APlayerState*> PlayerList = GS->PlayerArray;
	//	for (auto i : PlayerList)
	//	{
	//		ASPlayerState* PS = Cast<ASPlayerState>(i);
	//		if (PS)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("FOUND PLAYER: %s"), *PS->GetPlayerName());
	//		}
	//	}
	//}
}

/** spawns and initializes the PlayerState for this Controller */
// Called before possess and postlogin, only server runs this code
void ASPlayerController::InitPlayerState()
{
	Super::InitPlayerState();

}

void ASPlayerController::AllPostLogin()
{
	//UE_LOG(LogTemp, Warning, TEXT("PostLogin: %s"), *GetName());
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

// Self contained function in charge of setting initial values in different HUD objects
// PlayerState isn't valid here for clients, only for listen server
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
	
	// This works for listen server only but not any clients, playerstate is not valid yet for clients
	// So we run this code OnRep_PlayerState which works for clients but not for listen server
	AddPlayerToHUDScoreboard(PlayerState);
}

// Helper function, takes PlayerState reference and adds that PlayerState to scoreboard on HUD of owning client
void ASPlayerController::AddPlayerToHUDScoreboard(APlayerState* NewPlayerState)
{
	if (!NewPlayerState) { return; }
	// @TODO Listen server isn't running this code, need to fix
	// Add self stats to scoreboard now that our playerstate is valid to access
	ASPlayerState* PS = Cast<ASPlayerState>(NewPlayerState);
	if (PS && MyGameInfo)
	{
		MyGameInfo->AddPlayerToScoreboard(PS->GetPlayerName(), FString::FromInt(PS->PlayerKills), FString::FromInt(PS->PlayerDeaths), FString::SanitizeFloat(PS->Score));
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

		ASPlayerCharacter* MySPlayerChar = Cast<ASPlayerCharacter>(GetPawn());

		// If our current slot is a valid inventory index
		if (WeaponInventory.Num() > CurrentSlot)
		{
			if (MySPlayerChar)
			{
				// Equip whatever weapon is in the current slot if we are a SPlayerCharacter
				MySPlayerChar->ChangeWeapons(WeaponInventory[CurrentSlot], CurrentSlot);
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
	if (GetLocalRole() < ROLE_Authority)
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

			// Update HUD image for client
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

// When server updates CurrentSlot property, owning client updates their HUD
void ASPlayerController::OnRep_SlotChange()
{
	if (MyGameInfo) 
	{ 
		MyGameInfo->UpdateInventoryHUD(CurrentSlot);
	}
}

// When server PICKS UP NEW WEAPON, updates SlotToUpdate so owning client updates HUD for that slot
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

	// Server updates this property and owning client reacts by updating HUD showing newly "acquired" inventory item in slot
	DOREPLIFETIME_CONDITION(ASPlayerController, SlotToUpdate, COND_OwnerOnly);
}