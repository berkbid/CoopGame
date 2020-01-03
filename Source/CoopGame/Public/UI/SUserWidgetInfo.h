// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetInfo.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetInfo : public UUserWidget
{
	GENERATED_BODY()
	


protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SampleText;

	AActor* OwningActor;


public:

	void SetOwningActor(AActor* NewOwner);
	void SetSampleText(FString PlayerName);
	void SetTextVisibility(bool bIsVisible);
	
};
