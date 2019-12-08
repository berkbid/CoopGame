// Fill out your copyright notice in the Description page of Project Settings.


#include "SAIController.h"
#include "SCharacter.h"
#include "SWeapon.h"

void ASAIController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// If we possess a SCharacter, try to equip first weapon slot
	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	TSubclassOf<ASWeapon> FirstWeaponClass = WeaponInventory[0];

	if (MyPawn && FirstWeaponClass)
	{
		MyPawn->ChangeWeapons(FirstWeaponClass, 0);
	}
}