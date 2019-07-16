// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerUpSpeed.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPowerUpSpeed : public ASPowerupActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditDefaultsOnly, Category = "SpeedActor")
	TSubclassOf<class UCharacterMovementComponent> MovementComponentClass;

	UPROPERTY(Replicated)
	class UCharacterMovementComponent* MoveComp;

	UPROPERTY(ReplicatedUsing = OnRep_SpeedChange)
	float NewMaxSpeed;

	UFUNCTION()
	void OnRep_SpeedChange();


	float DefaultMaxWalkSpeed;

	void OnSpeedTick();

public:
	virtual void ActivatePowerup(AActor* PowerActor) override;
};
