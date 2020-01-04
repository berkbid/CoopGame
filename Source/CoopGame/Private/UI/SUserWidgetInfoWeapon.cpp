// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetInfoWeapon.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "SWeaponPickup.h"
#include "Components/Image.h"

void USUserWidgetInfoWeapon::SetOwningActor(AActor* NewOwner)
{
	Super::SetOwningActor(NewOwner);

	if (NewOwner)
	{
		//SetWeaponText(NewOwner->GetName());

		ASWeaponPickup* WP = Cast<ASWeaponPickup>(NewOwner);
		if (WP)
		{
			FLinearColor TempColor;

			switch (WP->WeaponInfo.WeaponRarity)
			{
			case EWeaponRarity::Common:
				TempColor = FLinearColor(.7f, .7f, .7f, 1.f);
				break;
			case EWeaponRarity::Uncommon:
				TempColor = FLinearColor(1.f, .3f, 1.f, 1.f);
				break;
			case EWeaponRarity::Rare:
				TempColor = FLinearColor(0.f, 0.f, 1.f, 1.f);
				break;
			case EWeaponRarity::Epic:
				TempColor = FLinearColor(1.f, 0.f, 1.f, 1.f);
				break;
			case EWeaponRarity::Legendary:
				TempColor = FLinearColor(1.f, 1.f, 0.f, 1.f);
				break;
			default:
				break;
			}
			SetWeaponBorderColor(TempColor);
			SetWeaponText(WP->WeaponInfo.WeaponName.ToString());
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

void USUserWidgetInfoWeapon::SetWeaponBorderColor(FLinearColor NewColor)
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