// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "UI/SUserWidgetInfo.h"
#include "SUserWidgetInfoWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetInfoWeapon : public USUserWidgetInfo
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* WeaponNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ClipText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* WeaponNameBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UBorder* WeaponSecondBorder;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* AmmoImage;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* StarImage;

	void SetWeaponText(FString PlayerName);

	void SetClipText(int32 ClipAmount);

	void SetBorderColors(FLinearColor NewColor);

	void SetAmmoImage(EAmmoType NewAmmoType);

	void SetStarImage(EWeaponRarity NewWeaponRarity);

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TMap<EAmmoType, class UTexture2D*> AmmoToTextureMap;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TMap<TEnumAsByte<EWeaponRarity>, class UTexture2D*> RarityToTextureMap;

public:

	virtual void SetOwningActor(AActor* NewOwner) override;
	
};
