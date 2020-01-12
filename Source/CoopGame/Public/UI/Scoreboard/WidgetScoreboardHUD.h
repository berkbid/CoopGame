// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetScoreboardHUD.generated.h"


class UWidgetScoreboardEntry;

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetScoreboardHUD : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UVerticalBox* ScoreboardVertical;

	/* Mapping from unique PlayerID to their scoreboard PlayerStats object reference */
	TMap<uint32, UWidgetScoreboardEntry*> ScoreboardDictionary;

	UPROPERTY(EditDefaultsOnly, Category = "Scoreboard")
	TSubclassOf<UWidgetScoreboardEntry> ScoreboardEntryClass;

public:
	void AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber);
	void UpdatePlayerScore(uint32 PlayerNumber, float NewScore);
	void UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills);
	void UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths);
};
