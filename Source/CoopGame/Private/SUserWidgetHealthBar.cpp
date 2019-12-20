// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetHealthBar.h"
#include "SHealthComponent.h"
#include "Components/ActorComponent.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "SPlayerState.h"
#include "SPlayerCharacter.h"
//#include "Runtime/UMG/Public/UMG.h"
//#include "Runtime/UMG/Public/UMGStyle.h"
//#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
//#include "Runtime/UMG/Public/IUMGModule.h"
//#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

// This user widget component is replicated I think because it exists on the pawn not the player controller
bool USUserWidgetHealthBar::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

//PlayerState isn't always valid on BeginPlayer and here
// All clients and server run this code!!! Called on beginplay of widgetcomponent
void USUserWidgetHealthBar::SetOwningActor(AActor* NewOwner)
{
	if (!NewOwner) { return; }
	// Skip repeated calls
	if (OwningActor == NewOwner) { return; }

	OwningActor = NewOwner;

	

	// Get health component reference of owner to bind to health changed event dispatcher
	USHealthComponent* HealthComp = Cast<USHealthComponent>(OwningActor->GetComponentByClass(USHealthComponent::StaticClass()));

	if (HealthComp)
	{
		// Remove the delegate before binding it. (Its always safe to unbind a delegate.)
		HealthComp->OnHealthChanged.RemoveDynamic(this, &USUserWidgetHealthBar::HandleHealthChanged);

		// Bind the delegate to update health bar
		HealthComp->OnHealthChanged.AddDynamic(this, &USUserWidgetHealthBar::HandleHealthChanged);

		// Set MaxHealth variable used to compute progress bar percent
		MaxHealth = HealthComp->DefaultHealth;

		if (HealthBar)
		{
			// Set initial health bar value
			HealthBar->SetPercent(HealthComp->GetHealth() / MaxHealth);
		}	
	}

	// Shouldn't try to do stuff with playerstate here because for listen server, the actors are never connected to player controllers at this point
	// so playerstate is invalid always for listen server
	APawn* OwningPawn = Cast<APawn>(OwningActor);
	// This PlayerState isn't always valid on begin play
	if (OwningPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("OwningActor: %s"), *OwningPawn->GetName());
		ASPlayerState* PS = Cast<ASPlayerState>(OwningPawn->GetPlayerState());

		if (PS)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Found PS: %s"), *PS->GetPlayerName());
			SetNameText(PS->GetPlayerName());
		}
		else
		{
			SetNameText(OwningPawn->GetName());
		}
	}
}

void USUserWidgetHealthBar::SetNameText(FString PlayerName)
{
	if (NameText)
	{
		NameText->SetText(FText::FromString(PlayerName));
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
