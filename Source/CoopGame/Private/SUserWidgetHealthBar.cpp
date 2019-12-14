// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetHealthBar.h"
#include "SHealthComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "SPlayerState.h"
//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

bool USUserWidgetHealthBar::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

// All clients and server run this code!!!
void USUserWidgetHealthBar::SetOwningActor(AActor* NewOwner)
{
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

	//Name is set different for players initially, but after respawn it gets corrected
	SetNameText(OwningActor->GetName());

	// Get health component reference of owner to bind to health changed event dispatcher
	USHealthComponent* HealthComp = Cast<USHealthComponent>(OwningActor->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthComp)
	{
		// Remove the delegate before binding it. (Its always safe to unbind a delegate.)
		HealthComp->OnHealthChanged.RemoveDynamic(this, &USUserWidgetHealthBar::HandleHealthChanged);

		// Bind the delegate from the PC that calls the BlueprintImplementableEvent.
		HealthComp->OnHealthChanged.AddDynamic(this, &USUserWidgetHealthBar::HandleHealthChanged);

		// Set MaxHealth variable used to compute progress bar percent
		MaxHealth = HealthComp->DefaultHealth;

		if (HealthBar)
		{
			// Set initial health bar value
			HealthBar->SetPercent(HealthComp->GetHealth() / MaxHealth);
		}
		
	}
}

void USUserWidgetHealthBar::SetNameText(FString NewName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(NewName));
	}
}

// This code runs for all clients and server
void USUserWidgetHealthBar::HandleHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Health / MaxHealth);
	}
}
