// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWeapon.h"
#include "SCharacter.h"
#include "Net/UnrealNetwork.h"


ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// Start with first slot as default
	CurrentSlot = 0;
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

		// Try equip weapon at currentslot
		if (WeaponInventory.Num() > CurrentSlot)
		{
			if (MyPawn)
			{
				MyPawn->ChangeWeapons(WeaponInventory[CurrentSlot], CurrentSlot);
			}
		}
		// If no weapon at currentslot, set currentslot back to 0 and if weapon in that spot, equip that weapon
		else
		{
			CurrentSlot = 0;
			if (WeaponInventory.Num() > CurrentSlot)
			{
				if (MyPawn)
				{
					MyPawn->ChangeWeapons(WeaponInventory[CurrentSlot], CurrentSlot);
				}
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
void ASPlayerController::SetCurrentWeapon(TSubclassOf<ASWeapon> WeaponClass, int WeaponSlot)
{
	if (MyGameInfo)
	{
		MyGameInfo->SetInventoryColor(WeaponSlot);
		MyGameInfo->SetInventoryImage(WeaponClass, WeaponSlot);
	}
}

void ASPlayerController::EquipSlotOne()
{
	ServerEquipWeapon(0);
}

void ASPlayerController::EquipSlotTwo()
{
	ServerEquipWeapon(1);
}

void ASPlayerController::EquipSlotThree()
{
	ServerEquipWeapon(2);
}

void ASPlayerController::EquipSlotFour()
{
	ServerEquipWeapon(3);
}

void ASPlayerController::EquipSlotFive()
{
	ServerEquipWeapon(4);
}

// MUST prefix with Server and require _Implementation
void ASPlayerController::ServerEquipWeapon_Implementation(int NewWeaponSlot)
{
	if (CurrentSlot == NewWeaponSlot) { return; }

	TSubclassOf<ASWeapon> NewWeaponClass = WeaponInventory[NewWeaponSlot];

	if (!NewWeaponClass) { return; }

	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	if (MyPawn)
	{
		MyPawn->ChangeWeapons(NewWeaponClass, NewWeaponSlot);
	}
	//update current slot even if not controlling character, and when possessing character, use this value to equip this slot
	CurrentSlot = NewWeaponSlot;
}

bool ASPlayerController::ServerEquipWeapon_Validate(int NewWeaponSlot)
{
	// This is for anti cheat stuff
	return true;
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ASPlayerController, WeaponInventory, COND_OwnerOnly);

}