// Fill out your copyright notice in the Description page of Project Settings.


#include "SWidgetCompPickupInfo.h"
#include "SUserWidgetInfo.h"

USWidgetCompPickupInfo::USWidgetCompPickupInfo()
{
	// Set common defaults when using widgets on Actors
	SetDrawAtDesiredSize(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetGenerateOverlapEvents(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}

void USWidgetCompPickupInfo::BeginPlay()
{
	Super::BeginPlay();
}

void USWidgetCompPickupInfo::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void USWidgetCompPickupInfo::InitWidget()
{
	Super::InitWidget();

	if (Widget)
	{
#if !UE_BUILD_SHIPPING
		if (!Widget->IsA(USUserWidgetInfo::StaticClass()))
		{
			// Suggest deriving from actor widget (not strictly required, but you lose the benefit of auto-assigning the Actor this way)
			//UE_LOG(LogGame, Warning, TEXT("WidgetClass for %s does not derive from SActorWidget"), *GetNameSafe(GetOwner()));
		}
#endif
		WidgetInfoInst = Cast<USUserWidgetInfo>(Widget);

		if (WidgetInfoInst)
		{
		}
	}
}

void USWidgetCompPickupInfo::UpdateWidgetText(FString PlayerName)
{
	if (WidgetInfoInst)
	{
		WidgetInfoInst->SetSampleText(PlayerName);
	}
}

void USWidgetCompPickupInfo::SetInfoVisible()
{
	if (WidgetInfoInst)
	{
		WidgetInfoInst->SetTextVisibility();
	}
}
