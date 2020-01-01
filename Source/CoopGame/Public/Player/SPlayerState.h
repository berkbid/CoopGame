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

	ASPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Called by Controller when its PlayerState is initially replicated. */
	virtual void ClientInitialize(class AController* C) override;

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddScore(float ScoreDelta);

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddKill();

	UFUNCTION(BlueprintCallable, Category = "PlayerState")
	void AddDeath();

	virtual void OnRep_Score() override;

	virtual void OnRep_PlayerName() override;

	UFUNCTION()
	void OnRep_PlayerKills();

	UFUNCTION()
	void OnRep_PlayerDeaths();

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Player")
	uint8 PlayerNumber;

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(ReplicatedUsing=OnRep_PlayerKills, VisibleAnywhere, Category = "Player")
	uint32 PlayerKills;

	// Could replicate this using a function so when a new player joins, ALL clients UPDATE their HUD OnRep_PlayerNumber
	UPROPERTY(ReplicatedUsing=OnRep_PlayerDeaths, VisibleAnywhere, Category = "Player")
	uint32 PlayerDeaths;


};
