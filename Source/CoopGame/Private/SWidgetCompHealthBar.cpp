// Fill out your copyright notice in the Description page of Project Settings.


#include "SWidgetCompHealthBar.h"
#include "SUserWidgetHealthBar.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerState.h"


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
	//UE_LOG(LogTemp, Warning, TEXT("Hello"));
	// Just call update name on health bar widget here
	//player state is not always valid at this point
	//APawn* OwningPawn = Cast<APawn>(GetOwner());

	//if (OwningPawn)
	//{
	//	ASPlayerState* PS = Cast<ASPlayerState>(OwningPawn->GetPlayerState());
	//	if (PS)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("FOUND PLAYERSTATE"));
	//	}
	//}

}

void USWidgetCompHealthBar::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

// call this code onrep_playerstate in player controller for every1
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
			
			// Assign the owner, now we have easy access in the widget itself
			HealthBarInst->SetOwningActor(GetOwner());
			// CHeck if we are an AI and set name

		}
	}
}


void USWidgetCompHealthBar::UpdateWidgetName(const FString& PlayerName)
{
	if (HealthBarInst)
	{
		HealthBarInst->SetNameText(PlayerName);
	}
}


