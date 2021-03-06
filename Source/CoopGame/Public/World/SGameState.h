// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	// This is default value
	WaitingToStart,

	WaveInProgress,

	// No longer spawning new bots, but waiting for remaining bots to die
	WaitingToComplete,

	GameOver,

	WaveComplete,
};


/**
 * 
 */
UCLASS()
class COOPGAME_API ASGameState : public AGameState
{
	GENERATED_BODY()
	
protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category = "GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_WaveState, Category = "GameState")
	EWaveState WaveState;

public:


	void SetWaveState(EWaveState NewState);

	void UpdateWaveStateHUD();

	FString GetWaveStateString();
};
