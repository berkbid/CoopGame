// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SUserWidgetInventoryInfo.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetInventoryInfo : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* InventoryBorder;

public:

	void AddInventoryWidget(class UVerticalBox* InventoryWidget);

};
