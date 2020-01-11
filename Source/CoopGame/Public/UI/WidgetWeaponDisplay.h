// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "Blueprint/UserWidget.h"
#include "WidgetWeaponDisplay.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API UWidgetWeaponDisplay : public UUserWidget
{
	GENERATED_BODY()
	

public:
	void InitWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo);

	void SetWeaponCurrentAmmo(int32 NewCurrentAmmo);

	void SetWeaponExtraAmmo(int32 NewExtraAmmo);

	void SetBothAmmo(int32 NewCurrentAmmo, int32 NewExtraAmmo);

	void QueryToSetExtraAmmo(EAmmoType NewAmmoType, int32 NewExtraAmmo);

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* CurrentAmmoBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentWeaponAmmo;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* CurrentWeaponText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AmmoTextures")
	TMap<EAmmoType, class UTexture2D*> AmmoToTextureMap;

	FWeaponInfo CurrentWeaponInfo;

	int32 ExtraClipSize;

	void SetAmmoText();

	void SetWeaponText();

	void SetAmmoImage();

};
