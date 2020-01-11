// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetInventoryHUD.h"
#include "WidgetInventorySlot.h"
#include "WidgetWeaponDisplay.h"
#include "WidgetAmmoDisplay.h"
#include "Engine/Texture2D.h"
#include "Components/UniformGridPanel.h"


bool UWidgetInventoryHUD::Initialize()
{

	return Super::Initialize();
}

void UWidgetInventoryHUD::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (SlotGrid)
	{
		// This re-calculates the array when any property changes just to make sure we have the correct info
		InventorySlots.Empty();
		TArray<UWidget*> ChildArray = SlotGrid->GetAllChildren();
		int32 ChildArrayLen = ChildArray.Num();

		for (int i = 0; i < ChildArrayLen; ++i)
		{
			InventorySlots.Add(Cast<UWidgetInventorySlot>(ChildArray[i]));
		}
	}
}

// Need to tell slot the weapon info and extra ammo
void UWidgetInventoryHUD::HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo, UTexture2D* WeaponTexture)
{
	// If we have a valid child at WeaponSlot, call InitSlot on that SOverlayInventorySlot class
	if (InventorySlots.Num() > WeaponSlot)
	{
		UWidgetInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			// Init slot with weapon info and extra ammo data
			NewSlot->InitSlot(WeaponTexture, NewWeaponInfo);
			NewSlot->UpdateExtraAmmo(CurrentAmmoInfo.GetAmmoTypeAmount(NewWeaponInfo.AmmoType));
		}
		// If we pickup weapon in our current slot, return true
		if (NewSlot == CurrentSlot)
		{
			// Query for extra ammo for current slot and update weapon info
			int32 NewExtraAmmo = GetExtraAmmoOfType(NewWeaponInfo.AmmoType);
			UpdateWeaponInfo(NewWeaponInfo, NewExtraAmmo);
		}
	}
}

void UWidgetInventoryHUD::HandleSlotChange(int32 WeaponSlot)
{
	// If we have a valid child at WeaponSlot index
	if (InventorySlots.Num() > WeaponSlot)
	{
		UWidgetInventorySlot* NewSlot = InventorySlots[WeaponSlot];
		if (NewSlot)
		{
			UpdateWeaponInfo(NewSlot->CurrentWeaponInfo, NewSlot->CurrentExtraAmmo);

			//// If we have a current slot selected, reset that slot
			if (CurrentSlot)
			{
				CurrentSlot->ResetSlot();
			}

			//// Activate new slot and set current slot to new slot
			NewSlot->ActivateSlot();
			CurrentSlot = NewSlot;
		}
	}
}

void UWidgetInventoryHUD::UpdateCurrentSlotAmmo(int32 CurrentAmmo)
{
	if (CurrentSlot)
	{
		CurrentSlot->UpdateCurrentAmmo(CurrentAmmo);
	}

	if (WeaponDisplay)
	{
		WeaponDisplay->SetWeaponCurrentAmmo(CurrentAmmo);
	}
}

void UWidgetInventoryHUD::InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo)
{
	CurrentAmmoInfo = StartingAmmoInfo;

	UpdateAmmoTypeAmount(EAmmoType::MiniAmmo, StartingAmmoInfo.MiniCount);
	UpdateAmmoTypeAmount(EAmmoType::MediumAmmo, StartingAmmoInfo.MediumCount);
	UpdateAmmoTypeAmount(EAmmoType::HeavyAmmo, StartingAmmoInfo.HeavyCount);
	UpdateAmmoTypeAmount(EAmmoType::ShellAmmo, StartingAmmoInfo.ShellCount);
	UpdateAmmoTypeAmount(EAmmoType::RocketAmmo, StartingAmmoInfo.RocketCount);

}

int32 UWidgetInventoryHUD::GetExtraAmmoOfType(EAmmoType NewAmmoType)
{
	return CurrentAmmoInfo.GetAmmoTypeAmount(NewAmmoType);
}

void UWidgetInventoryHUD::UpdateAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	CurrentAmmoInfo.SetAmmoTypeAmount(NewAmmoType, NewExtraAmmo);

	for (UWidgetInventorySlot* InvSlot : InventorySlots)
	{
		if (InvSlot && InvSlot->CurrentWeaponInfo.AmmoType == NewAmmoType)
		{
			InvSlot->UpdateExtraAmmo(NewExtraAmmo);
		}
	}

	if (WeaponDisplay)
	{
		WeaponDisplay->QueryToSetExtraAmmo(NewAmmoType, NewExtraAmmo);
	}

	switch (NewAmmoType)
	{
	case EAmmoType::MiniAmmo:
		SetMiniAmmoText(FString::FromInt(NewExtraAmmo));
		break;
	case EAmmoType::MediumAmmo:
		SetMediumAmmoText(FString::FromInt(NewExtraAmmo));
		break;
	case EAmmoType::HeavyAmmo:
		SetHeavyAmmoText(FString::FromInt(NewExtraAmmo));
		break;
	case EAmmoType::ShellAmmo:
		SetShellAmmoText(FString::FromInt(NewExtraAmmo));
		break;
	case EAmmoType::RocketAmmo:
		SetRocketAmmoText(FString::FromInt(NewExtraAmmo));
		break;
	default:
		break;
	}
}

void UWidgetInventoryHUD::UpdateWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	if (WeaponDisplay)
	{
		WeaponDisplay->InitWeaponInfo(NewWeaponInfo, NewExtraAmmo);
	}
}

void UWidgetInventoryHUD::SetMiniAmmoText(FString NewText)
{
	if (MiniText)
	{
		MiniText->SetAmmoText(FText::FromString("Mini: " + NewText + "/" + FString::FromInt(CurrentAmmoInfo.MaxMiniAmmo)));
	}
}

void UWidgetInventoryHUD::SetMediumAmmoText(FString NewText)
{
	if (MediumText)
	{
		MediumText->SetAmmoText(FText::FromString("Medium: " + NewText + "/" + FString::FromInt(CurrentAmmoInfo.MaxMediumAmmo)));
	}
}

void UWidgetInventoryHUD::SetHeavyAmmoText(FString NewText)
{
	if (HeavyText)
	{
		HeavyText->SetAmmoText(FText::FromString("Heavy: " + NewText + "/" + FString::FromInt(CurrentAmmoInfo.MaxHeavyAmmo)));
	}
}

void UWidgetInventoryHUD::SetShellAmmoText(FString NewText)
{
	if (ShellText)
	{
		ShellText->SetAmmoText(FText::FromString("Shell: " + NewText + "/" + FString::FromInt(CurrentAmmoInfo.MaxShellAmmo)));
	}
}

void UWidgetInventoryHUD::SetRocketAmmoText(FString NewText)
{
	if (RocketText)
	{
		RocketText->SetAmmoText(FText::FromString("Rocket: " + NewText + "/" + FString::FromInt(CurrentAmmoInfo.MaxRocketAmmo)));
	}
}