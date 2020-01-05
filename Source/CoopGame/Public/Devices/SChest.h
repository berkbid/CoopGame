// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Devices/SContainer.h"
#include "SChest.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASChest : public ASContainer
{
	GENERATED_BODY()


public:
	ASChest();

	virtual void Interact(AActor* InteractedActor) override;

	virtual void ShowItemInfo(bool bIsVisible) override;


protected:

	UPROPERTY(EditAnywhere, Category="Items")
	TArray<TSubclassOf<class ASWeaponPickup>> WeaponArray;

	virtual void OnRep_OpenContainer() override;

};
