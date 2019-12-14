// Fill out your copyright notice in the Description page of Project Settings.


#include "SWidgetCompHealthBar.h"
#include "SUserWidgetHealthBar.h"




USWidgetCompHealthBar::USWidgetCompHealthBar()
{
	// Set common defaults when using widgets on Actors
	SetDrawAtDesiredSize(true);
	SetWidgetSpace(EWidgetSpace::Screen);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
		USUserWidgetHealthBar* WidgetInst = Cast<USUserWidgetHealthBar>(Widget);

		if (WidgetInst)
		{
			
			// Assign the owner, now we have easy access in the widget itself
			WidgetInst->SetOwningActor(GetOwner());

		}
	}
}

void USWidgetCompHealthBar::UpdateNameText(FString NewName)
{
	USUserWidgetHealthBar* WidgetInst = Cast<USUserWidgetHealthBar>(Widget);

	if (WidgetInst)
	{
		WidgetInst->SetNameText(NewName);
	}
}
