// Fill out your copyright notice in the Description page of Project Settings.


#include "SVerticalBoxCurrentWeapon.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"

USVerticalBoxCurrentWeapon::USVerticalBoxCurrentWeapon()
{

}

void USVerticalBoxCurrentWeapon::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	CurrentWeaponAmmo = nullptr;
	CurrentWeaponType = nullptr;
}

void USVerticalBoxCurrentWeapon::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Get reference to our children here for use during play
	CurrentWeaponAmmo = Cast<UOverlay>(GetChildAt(0));
	CurrentWeaponType = Cast<UBorder>(GetChildAt(1));
}

void USVerticalBoxCurrentWeapon::SetWeaponName(FString NewWeaponName)
{
	if (CurrentWeaponType)
	{
		UTextBlock* WeaponText = Cast<UTextBlock>(CurrentWeaponType->GetChildAt(0));
		if (WeaponText)
		{
			WeaponText->SetText(FText::FromString(NewWeaponName));
		}
	}
}

void USVerticalBoxCurrentWeapon::SetWeaponAmmo(int32 CurrentAmmo, int32 TotalAmmo)
{
	if (CurrentWeaponAmmo)
	{
		UTextBlock* AmmoText = Cast<UTextBlock>(CurrentWeaponAmmo->GetChildAt(1));
		if (AmmoText)
		{
			//FString NewAmmoText = FString::FromInt(CurrentAmmo) + " / " + FString::FromInt(TotalAmmo);
			AmmoText->SetText(FText::FromString(FString::FromInt(CurrentAmmo) + " / " + FString::FromInt(TotalAmmo)));
		}
	}
}
