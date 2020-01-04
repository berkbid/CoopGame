// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	class UBorder* WeaponNameBorder;

	void SetWeaponText(FString PlayerName);

	void SetWeaponBorderColor(FLinearColor NewColor);

public:

	virtual void SetOwningActor(AActor* NewOwner) override;
	
};
