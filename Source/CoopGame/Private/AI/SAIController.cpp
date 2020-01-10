// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"
#include "SCharacter.h"
#include "SWeapon.h"

ASAIController::ASAIController(const FObjectInitializer& ObjectInitializer)
{
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;

	CurrentSlot = -1;
	WeaponInventory.Init(FWeaponInfo(), InventoryMaxSize);
	AmmoInventory = FAmmoInfo(100, 100, 100, 100, 100, 300, 300, 300, 300, 300);
}

void ASAIController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	CurrentSlot = 0;

	// If we possess a SCharacter, try to equip first weapon slot
	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	if (!MyPawn) { return; }

	// Don't try to access invalid index
	if (WeaponInventory.Num() > CurrentSlot)
	{
		MyPawn->EquipWeaponClass(WeaponInventory[CurrentSlot].WeaponClass, WeaponInventory[CurrentSlot].CurrentAmmo);
	}
}