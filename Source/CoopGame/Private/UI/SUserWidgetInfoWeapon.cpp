// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfoWeapon.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "SWeaponPickup.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void USUserWidgetInfoWeapon::SetOwningActor(AActor* NewOwner)
{
	Super::SetOwningActor(NewOwner);

	if (NewOwner)
	{
		ASWeaponPickup* WP = Cast<ASWeaponPickup>(NewOwner);
		if (WP)
		{
			FLinearColor TempColor;
			FString TempRarityString;

			switch (WP->WeaponInfo.WeaponRarity)
			{
			case EWeaponRarity::Common:
				TempColor = FLinearColor(.7f, .7f, .7f, 1.f);
				TempRarityString = "Common ";
				
				break;
			case EWeaponRarity::Uncommon:
				TempColor = FLinearColor(1.f, .3f, 1.f, 1.f);
				TempRarityString = "UnCommon ";
				break;
			case EWeaponRarity::Rare:
				TempColor = FLinearColor(0.f, 0.f, 1.f, 1.f);
				TempRarityString = "Rare ";
				break;
			case EWeaponRarity::Epic:
				TempColor = FLinearColor(1.f, 0.f, 1.f, 1.f);
				TempRarityString = "Epic ";
				break;
			case EWeaponRarity::Legendary:
				TempColor = FLinearColor(1.f, 1.f, 0.f, 1.f);
				TempRarityString = "Legendary ";
				break;
			default:
				break;
			}

			SetBorderColors(TempColor);
			SetWeaponText(TempRarityString + WP->WeaponInfo.WeaponName.ToString());
			SetClipText(WP->WeaponInfo.CurrentAmmo);
			SetAmmoImage(WP->WeaponInfo.AmmoType);
			SetStarImage(WP->WeaponInfo.WeaponRarity);
		}
	}
}

void USUserWidgetInfoWeapon::SetWeaponText(FString PlayerName)
{
	if (WeaponNameText)
	{
		WeaponNameText->SetText(FText::FromString(PlayerName));
	}
}

void USUserWidgetInfoWeapon::SetClipText(int32 ClipAmount)
{
	if (ClipText)
	{
		ClipText->SetText(FText::FromString(FString::FromInt(ClipAmount)));
	}
}

void USUserWidgetInfoWeapon::SetBorderColors(FLinearColor NewColor)
{
	if (WeaponNameBorder)
	{
		WeaponNameBorder->SetBrushColor(NewColor);
		
	}

	if (WeaponSecondBorder)
	{
		NewColor.R /= 3.f;
		NewColor.G /= 3.f;
		NewColor.B /= 3.f;
		WeaponSecondBorder->SetBrushColor(NewColor);
	}
}

void USUserWidgetInfoWeapon::SetAmmoImage(EAmmoType NewAmmoType)
{
	if (!AmmoImage) { return; }

	UTexture2D** TempAmmoTexture = AmmoToTextureMap.Find(NewAmmoType);

	if (TempAmmoTexture)
	{
		if (UTexture2D* NewAmmoTexture = *TempAmmoTexture)
		{
			AmmoImage->SetBrushFromTexture(NewAmmoTexture);
		}
	}
}

void USUserWidgetInfoWeapon::SetStarImage(EWeaponRarity NewWeaponRarity)
{
	if (!StarImage) { return; }

	UTexture2D** TempRarityTexture = RarityToTextureMap.Find(NewWeaponRarity);

	if (TempRarityTexture)
	{
		if (UTexture2D* NewStarTexture = *TempRarityTexture)
		{
			StarImage->SetBrushFromTexture(NewStarTexture);
		}
	}
}