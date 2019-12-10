// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"


/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	virtual void OnRep_Score() override;

	virtual void OnRep_PlayerName() override;

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player")
	uint8 PlayerNumber;

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player")
	uint8 PlayerKills;

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player")
	uint8 PlayerDeaths;

};
