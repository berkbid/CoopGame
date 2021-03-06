// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventoryPageContainer.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetInventoryPageContainer : public UUserWidget
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UWidgetInventoryUI* InventoryUI;

public:
	void InitContainerInventory();

	void GetInventorySlotsRef(const TArray<class UWidgetInventorySlot*>& InventorySlotArrayRef);
};
