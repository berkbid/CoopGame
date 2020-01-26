// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventoryPageContainer.h"
#include "WidgetInventoryUI.h"
#include "WidgetInventorySlot.h"

void UWidgetInventoryPageContainer::GetInventorySlotsRef(const TArray<class UWidgetInventorySlot*>& InventorySlotArrayRef)
{
	if (InventoryUI)
	{
		InventoryUI->GetInventorySlotsRef(InventorySlotArrayRef);
	}
}

void UWidgetInventoryPageContainer::InitContainerInventory()
{
	if (InventoryUI)
	{
		InventoryUI->InitInventorySlots();
	}
}