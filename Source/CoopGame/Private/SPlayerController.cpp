// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWeapon.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;
	// Start with invalid first slot, so when it is set to 0 on possess, we replicate this change so client can update HUD
	CurrentSlot = 0;
	SlotToUpdate = -1;
	WeaponInventory.Init(NULL, InventoryMaxSize);
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Only local controllers can add widgets
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (!wGameInfo) { return; }
		MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);
		if (!MyGameInfo) { return; }
		

		MyGameInfo->SetOwningController(this);
		MyGameInfo->AddToViewport();

		for (int32 i = 0; i != WeaponInventory.Num(); ++i)
		{
			if (WeaponInventory[i])
			{
				// Must set color "active" inventory slot here because first OnPossess happens before beginplay
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
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//check(InputComponent);

	InputComponent->BindAction("Weapon1", IE_Pressed, this, &ASPlayerController::EquipSlotOne);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &ASPlayerController::EquipSlotTwo);
	InputComponent->BindAction("Weapon3", IE_Pressed, this, &ASPlayerController::EquipSlotThree);
	InputComponent->BindAction("Weapon4", IE_Pressed, this, &ASPlayerController::EquipSlotFour);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);

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

void ASPlayerController::SetScoreText(float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->HandleScoreChanged(NewScore);
	}
}

void ASPlayerController::EquipSlotOne()
{
	if (CurrentSlot == 0) { return; }
	ServerEquipWeapon(0);
}

void ASPlayerController::EquipSlotTwo()
{
	if (CurrentSlot == 1) { return; }
	ServerEquipWeapon(1);
}

void ASPlayerController::EquipSlotThree()
{
	if (CurrentSlot == 2) { return; }
	ServerEquipWeapon(2);
}

void ASPlayerController::EquipSlotFour()
{
	if (CurrentSlot == 3) { return; }
	ServerEquipWeapon(3);
}

void ASPlayerController::EquipSlotFive()
{
	if (CurrentSlot == 4) { return; }
	ServerEquipWeapon(4);
}

// Only server should be inside this call
bool ASPlayerController::PickedUpNewWeapon(TSubclassOf<ASWeapon> WeaponClass)
{
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
			OnRep_SlotToUpdate();

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


// MUST prefix with Server and require _Implementation
void ASPlayerController::ServerEquipWeapon_Implementation(int NewWeaponSlot)
{
	// Replicated, Update current slot even if we have no weapon in that slot
	CurrentSlot = NewWeaponSlot;
	OnRep_SlotChange();

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

bool ASPlayerController::ServerEquipWeapon_Validate(int NewWeaponSlot)
{
	// This is for anti cheat stuff
	return true;
}

// Replicated actions for death
void ASPlayerController::OnRep_SlotChange()
{
	if (MyGameInfo) 
	{ 
		MyGameInfo->UpdateInventoryHUD(CurrentSlot);
	}
}

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

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, WeaponInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASPlayerController, CurrentSlot, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASPlayerController, SlotToUpdate, COND_OwnerOnly);

}