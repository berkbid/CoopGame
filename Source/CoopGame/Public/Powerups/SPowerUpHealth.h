// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerUpHealth.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPowerUpHealth : public ASPowerupActor
{
	GENERATED_BODY()

protected:

	UPROPERTY(EditDefaultsOnly, Category = "HealthActor")
	TSubclassOf<class ASCharacter> CharClass;

	UPROPERTY(EditDefaultsOnly, Category = "HealthActor")
	TSubclassOf<class USHealthComponent> HealthComponentClass;

	UPROPERTY(EditDefaultsOnly, Category = "HealthActor")
	float HealAmount;

public:
	ASPowerUpHealth();

	virtual void OnTickPowerup() override;
	
};
