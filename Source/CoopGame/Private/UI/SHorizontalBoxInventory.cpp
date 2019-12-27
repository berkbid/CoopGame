// Fill out your copyright notice in the Description page of Project Settings.


#include "SHorizontalBoxInventory.h"
#include "SOverlayInventorySlot.h"
#include "Engine/Texture2D.h"


USHorizontalBoxInventory::USHorizontalBoxInventory()
{

}

void USHorizontalBoxInventory::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// This re-calculates the array when any property changes just to make sure we have the correct info
	InventorySlots.Empty();
	TArray<UWidget*> ChildArray = GetAllChildren();
	int32 ChildArrayLen = ChildArray.Num();

	for (int i = 0; i < ChildArrayLen; ++i)
	{
		InventorySlots.Add(Cast<USOverlayInventorySlot>(ChildArray[i]));
	}
}

void USHorizontalBoxInventory::HandlePickupWeapon(UTexture2D* WeaponTexture, int WeaponSlot)
{
	// If we have a valid child at WeaponSlot, call InitSlot on that SOverlayInventorySlot class
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			NewSlot->InitSlot(WeaponTexture);
		}
	}
}

void USHorizontalBoxInventory::HandleSlotChange(int WeaponSlot)
{
	// If we have a valid child at WeaponSlot index
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			// If we have a current slot selected, reset that slot
			if (CurrentSlot)
			{
				CurrentSlot->ResetSlot();
			}

			// Activate new slot and set current slot to new slot
			NewSlot->ActivateSlot();
			CurrentSlot = NewSlot;
		}
	}
}
