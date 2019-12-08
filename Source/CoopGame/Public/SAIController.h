// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SAIController.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASAIController : public AAIController
{
	GENERATED_BODY()
	

protected:
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TArray<TSubclassOf<class ASWeapon>> WeaponInventory;

	/* Keep track of which weapon slot is currently equipped */
	int CurrentSlot;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int InventoryMaxSize;

	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	int CurrentInventorySize;

public:
	ASAIController(const FObjectInitializer& ObjectInitializer);
};
