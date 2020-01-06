// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeaponTypes.h"
#include "UI/ItemInfoWidgets/SUserWidgetInfo.h"
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

	void SetAmmoImage(EAmmoType NewAmmoType);

public:

	virtual void SetOwningActor(AActor* NewOwner) override;

	void SetAmmoText(int32 NewAmmo);
	
};
