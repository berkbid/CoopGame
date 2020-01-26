// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventoryPage.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetInventoryPage : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetInventoryPageContainer* InventoryContainer;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetInventoryItemDisplay* InventoryItemDisplay;

public:

	void InitInventoryPage();

	void GetInventorySlotReferences(const TArray<class UWidgetInventorySlot*>& InventorySlotArrayRef);
};
