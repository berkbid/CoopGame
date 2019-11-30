// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetHealthBar.h"
#include "SHealthComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
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

void USUserWidgetHealthBar::SetOwningActor(AActor* NewOwner)
{
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

	// Update player name text on widget
	if (NameText)
	{
		NameText->SetText(FText::FromString(OwningActor->GetName()));
	}

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

		// Set initial health bar value
		// Update HealthBar manually instead of calling BlueprintImplementableEvent
		HealthBar->SetPercent(HealthComp->GetHealth() / MaxHealth);
		//UpdateHealthBar(HealthComp);
	}

}

void USUserWidgetHealthBar::HandleHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Update HealthBar manually instead of calling BlueprintImplementableEvent
	HealthBar->SetPercent(Health / MaxHealth);
	//UpdateHealthBar(HealthComp);

}
