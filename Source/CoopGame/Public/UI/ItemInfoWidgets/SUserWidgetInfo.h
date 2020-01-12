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
	class UBorder* InfoBorder;

	AActor* OwningActor;


public:

	virtual void SetOwningActor(AActor* NewOwner);
	void SetWidgetVisibility(bool bVisible);
	
};
