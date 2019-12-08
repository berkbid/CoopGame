// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWeapon.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Start with invalid first slot, so when it is set to 0 on possess, we replicate this change so client can update HUD
	CurrentSlot = -1;
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Only local controllers can add widgets
	//if (Role < ROLE_Authority)
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (wGameInfo)
		{
			MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);

			if (MyGameInfo)
			{
				MyGameInfo->SetOwningController(this);
				MyGameInfo->AddToViewport();


				// Instead we can loop over our array of weaponclasses and update HUD
				for (int32 i = 0; i != WeaponInventory.Num(); ++i)
				{
					if (WeaponInventory[i])
					{
						
						if (i == CurrentSlot)
						{
							MyGameInfo->SetInventoryColor(CurrentSlot);
						}
						MyGameInfo->SetInventoryImage(WeaponInventory[i], i);
					}
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
		// If we possess a SCharacter, try to equip first weapon slot if it has a weapon
		ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());

		// Reset Currently Equipped Slot back to 0 on possess new pawn
		CurrentSlot = 0;

		// Try equip weapon at currentslot
		if (WeaponInventory.Num() > CurrentSlot)
		{
			// Try to equip weapon at slot if it exists
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

// Client should call this, not server
void ASPlayerController::SetInventorySlotImage(TSubclassOf<ASWeapon> WeaponClass, int WeaponSlot)
{
	if (MyGameInfo)
	{
		MyGameInfo->SetInventoryImage(WeaponClass, WeaponSlot);
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

// MUST prefix with Server and require _Implementation
void ASPlayerController::ServerEquipWeapon_Implementation(int NewWeaponSlot)
{
	// Replicated, Update current slot even if we have no weapon in that slot
	CurrentSlot = NewWeaponSlot;
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
	if (MyGameInfo) { MyGameInfo->SetInventoryColor(CurrentSlot); }
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, WeaponInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ASPlayerController, CurrentSlot, COND_OwnerOnly);

}