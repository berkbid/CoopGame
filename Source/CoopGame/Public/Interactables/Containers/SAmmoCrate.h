// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/Containers/SContainer.h"
#include "SAmmoCrate.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASAmmoCrate : public ASContainer
{
	GENERATED_BODY()
	

public:
	ASAmmoCrate();

	virtual void Interact(class APlayerController* InteractedPC) override;


protected:

	virtual void OnRep_OpenContainer() override;

protected:
	UPROPERTY(EditAnywhere, Category = "Ammo", meta = (ClampMin = 0, ClampMax = 5))
	uint8 NumberOfAmmoPickups;
};
