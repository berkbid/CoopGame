// Fill out your copyright notice in the Description page of Project Settings.


#include "SHorizontalBoxInventory.h"
#include "SOverlayInventorySlot.h"
#include "Engine/Texture2D.h"


void USHorizontalBoxInventory::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	InventorySlots.Empty();

}

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

void USHorizontalBoxInventory::HandlePickupWeapon(int32 WeaponSlot, UTexture2D* WeaponTexture, int32 SlotTotal)
{
	// If we have a valid child at WeaponSlot, call InitSlot on that SOverlayInventorySlot class
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			NewSlot->InitSlot(WeaponTexture, SlotTotal);
		}
	}
}

void USHorizontalBoxInventory::HandleSlotChange(int32 WeaponSlot)
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

void USHorizontalBoxInventory::SetSlotAmmo(int32 WeaponSlot, int32 SlotTotal)
{
	if (WeaponSlot < 0) { return; }
	// If we have a valid child at WeaponSlot index
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			NewSlot->UpdateAmmoText(SlotTotal);
		}
	}
}
