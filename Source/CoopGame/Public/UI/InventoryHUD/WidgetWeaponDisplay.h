// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetWeaponDisplay.generated.h"


class UTexture2D;
class UTextBlock;

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetWeaponDisplay : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo, UTexture2D* AmmoTexture);

	void QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo);

	void SetAmmoText(int32 NewCurrentAmmo);

	void SetWeaponText(FName NewWeaponName);

	void SetAmmoImage(EAmmoType NewAmmoType, UTexture2D* AmmoTexture);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* CurrentAmmoBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentWeaponAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* CurrentWeaponText;

	FWeaponInfo CurrentWeaponInfo;

	int32 ExtraClipSize;

	int32 TempCurrentAmmo;

};
