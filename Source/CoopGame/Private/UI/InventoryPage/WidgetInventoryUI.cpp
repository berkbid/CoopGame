// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventoryUI.h"
#include "Components/UniformGridPanel.h"
#include "WidgetInventorySlot.h"
#include "Components/Border.h"

void UWidgetInventoryUI::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (SlotGrid)
	{
		// This re-calculates the array when any property changes just to make sure we have the correct info
		InventoryBorders.Empty();
		TArray<UWidget*> ChildArray = SlotGrid->GetAllChildren();
		int32 ChildArrayLen = ChildArray.Num();

		for (int i = 0; i < ChildArrayLen; ++i)
		{
			InventoryBorders.Add(Cast<UBorder>(ChildArray[i]));
		}
	}
}

void UWidgetInventoryUI::InitInventorySlots()
{
	int32 InvLen = InventorySlotsRef.Num();
	if (InventoryBorders.Num() >= InvLen)
	{
		for (int i = 0; i < InvLen; ++i)
		{
			UWidgetInventorySlot* SlotTemp = InventorySlotsRef[i];
			if (SlotTemp)
			{
				SlotTemp->RemoveFromParent();
				SlotTemp->SetVisibility(ESlateVisibility::Visible);
				InventoryBorders[i]->AddChild(SlotTemp);
			}
		}
	}
}

void UWidgetInventoryUI::GetInventorySlotsRef(const TArray<UWidgetInventorySlot*>& InventorySlotArrayRef)
{
	InventorySlotsRef = InventorySlotArrayRef;
}