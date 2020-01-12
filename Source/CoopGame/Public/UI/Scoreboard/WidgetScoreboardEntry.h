// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetScoreboardEntry.generated.h"


class UTextBlock;

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetScoreboardEntry : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* KillsText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* DeathText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ScoreText;

public:
	void InitEntry(FString NewPlayerName);

	void SetNameText(FString NewName);

	void SetScoreText(FString NewScore);

	void SetKillText(FString NewKills);

	void SetDeathText(FString NewDeath);
};
