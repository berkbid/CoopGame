// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventoryPage.h"
#include "WidgetInventoryItemDisplay.h"
#include "WidgetInventoryPageContainer.h"
#include "WidgetInventorySlot.h"

void UWidgetInventoryPage::InitInventoryPage()
{
	if (InventoryContainer)
	{
		InventoryContainer->InitContainerInventory();
	}
}

void UWidgetInventoryPage::GetInventorySlotReferences(const TArray<UWidgetInventorySlot*>& InventorySlotArrayRef)
{
	if (InventoryContainer)
	{
		InventoryContainer->GetInventorySlotsRef(InventorySlotArrayRef);
	}
}