// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SWidgetCompHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USWidgetCompHealthBar : public UWidgetComponent
{
	GENERATED_BODY()
	

public:

	virtual void InitWidget() override;

	USWidgetCompHealthBar();
};
