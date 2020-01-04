// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "SWidgetCompPickupInfo.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API USWidgetCompPickupInfo : public UWidgetComponent
{
	GENERATED_BODY()
	

public:
	USWidgetCompPickupInfo();

	/** UActorComponent Interface */
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitWidget() override;

	void SetInfoVisible(bool bIsVisible);

	class USUserWidgetInfo* WidgetInfoInst;

protected:

	
};
