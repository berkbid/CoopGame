// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetInventoryUI.generated.h"

class UBorder;

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetInventoryUI : public UUserWidget
{
	GENERATED_BODY()
	

protected:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UUniformGridPanel* SlotGrid;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder1;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder2;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder3;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder4;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder5;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UBorder* SlotBorder6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TArray<UBorder*> InventoryBorders;

	TArray<class UWidgetInventorySlot*> InventorySlotsRef;

public:

	virtual void SynchronizeProperties() override;

	void InitInventorySlots();

	void GetInventorySlotsRef(const TArray<class UWidgetInventorySlot*>& InventorySlotArrayRef);

};
