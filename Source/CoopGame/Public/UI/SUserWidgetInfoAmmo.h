// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "UI/SUserWidgetInfo.h"
#include "SUserWidgetInfoAmmo.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USUserWidgetInfoAmmo : public USUserWidgetInfo
{
	GENERATED_BODY()
	

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* AmmoAmountText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UImage* AmmoImage;

	UPROPERTY(EditDefaultsOnly, Category = "Ammo")
	TMap<EAmmoType, class UTexture2D*> AmmoToTextureMap;

	void SetAmmoImage(UTexture2D* NewTexture);

public:

	virtual void SetOwningActor(AActor* NewOwner) override;

	void SetAmmoText(int32 NewAmmo);
	
};
