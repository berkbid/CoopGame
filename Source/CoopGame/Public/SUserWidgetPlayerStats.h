// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetPlayerStats.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetPlayerStats : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* NameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* KillsText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* DeathText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ScoreText;
};
