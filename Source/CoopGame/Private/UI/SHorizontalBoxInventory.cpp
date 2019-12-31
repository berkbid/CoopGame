// Fill out your copyright notice in the Description page of Project Settings.


#include "SHorizontalBoxInventory.h"
#include "SOverlayInventorySlot.h"
#include "Engine/Texture2D.h"
#include "SPlayerController.h"


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

void USHorizontalBoxInventory::HandlePickupWeapon(int32 WeaponSlot, UTexture2D* WeaponTexture, int32 CurrentAmmo, int32 ExtraAmmo, EAmmoType NewAmmoType)
{
	// If we have a valid child at WeaponSlot, call InitSlot on that SOverlayInventorySlot class
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			NewSlot->InitSlot(WeaponTexture, CurrentAmmo, ExtraAmmo, NewAmmoType);
		}
	}
}

void USHorizontalBoxInventory::HandleSlotChange(int32 WeaponSlot, int32& SlotCurrentAmmo, int32& SlotExtraAmmo)
{
	// If we have a valid child at WeaponSlot index
	if (InventorySlots.Num() > WeaponSlot)
	{
		USOverlayInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			SlotCurrentAmmo = NewSlot->CurrentClipAmmo;
			SlotExtraAmmo = NewSlot->CurrentExtraAmmo;

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

// @TODO Figure out how to handle extra ammo updates
void USHorizontalBoxInventory::UpdateCurrentSlotAmmo(int32 CurrentAmmo)
{
	if (CurrentSlot)
	{
		CurrentSlot->UpdateCurrentAmmo(CurrentAmmo);
	}
}

void USHorizontalBoxInventory::UpdateExtraSlotAmmo(int32 ExtraAmmo)
{
	if (CurrentSlot)
	{
		CurrentSlot->UpdateExtraAmmo(ExtraAmmo);
	}
}

void USHorizontalBoxInventory::UpdateBothAmmo(int32 CurrentAmmo, int32 ExtraAmmo)
{
	if (CurrentSlot)
	{
		CurrentSlot->UpdateBothAmmo(CurrentAmmo, ExtraAmmo);
	}
}

void USHorizontalBoxInventory::UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	for (USOverlayInventorySlot* InvSlot : InventorySlots)
	{
		if (InvSlot)
		{
			if (InvSlot->CurrentAmmoType == NewAmmoType)
			{
				InvSlot->UpdateExtraAmmo(NewExtraAmmo);
			}
		}
	}
}
