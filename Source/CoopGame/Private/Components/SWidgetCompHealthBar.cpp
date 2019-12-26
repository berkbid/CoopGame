// Fill out your copyright notice in the Description page of Project Settings.


#include "SWidgetCompHealthBar.h"
#include "SUserWidgetHealthBar.h"
#include "Net/UnrealNetwork.h"

USWidgetCompHealthBar::USWidgetCompHealthBar()
{
	// Set common defaults when using widgets on Actors
	SetDrawAtDesiredSize(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetGenerateOverlapEvents(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);

	
}

// The Super() calls InitWidget()
void USWidgetCompHealthBar::BeginPlay()
{
	Super::BeginPlay();

}

void USWidgetCompHealthBar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// This code is replicated to everyone
void USWidgetCompHealthBar::InitWidget()
{
	// Base implementation creates the 'Widget' instance
	Super::InitWidget();
	
	if (Widget)
	{
#if !UE_BUILD_SHIPPING
		if (!Widget->IsA(USUserWidgetHealthBar::StaticClass()))
		{
			// Suggest deriving from actor widget (not strictly required, but you lose the benefit of auto-assigning the Actor this way)
			//UE_LOG(LogGame, Warning, TEXT("WidgetClass for %s does not derive from SActorWidget"), *GetNameSafe(GetOwner()));
		}
#endif
		HealthBarInst = Cast<USUserWidgetHealthBar>(Widget);

		if (HealthBarInst)
		{
			APawn* OwningPawn = Cast<APawn>(GetOwner());
			if (OwningPawn)
			{
				// Assign the owner, now we have easy access in the widget itself
				HealthBarInst->SetOwningActor(OwningPawn);

			}
		}
	}
}

void USWidgetCompHealthBar::UpdateWidgetName(FString PlayerName)
{
	if (HealthBarInst)
	{
		HealthBarInst->SetNameText(PlayerName);
	}
}


