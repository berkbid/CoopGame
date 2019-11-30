// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"

bool USUserWidgetGameInfo::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

void USUserWidgetGameInfo::SetOwningController(APlayerController* NewController)
{
	// Skip repeated calls
	if (OwningController == NewController) { return; }

	OwningController = NewController;

	// Bind to delegate on controller not playerstate
	// Save player state?
	//ASPlayerState* PS = Cast<ASPlayerState>(OwningController->PlayerState);

	//if (PS)
	//{
		//PS->OnScoreChanged.AddDynamic(this, &USUserWidgetGameInfo::HandleScoreChanged);

		// Set initial score text
		//HandleScoreChanged(PS->Score);
	//}

}

void USUserWidgetGameInfo::HandleScoreChanged(float NewScore)
{
	if (ScoreText)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Really changing score: %f"), NewScore);
		FString tempString("SCORE: " + FString::SanitizeFloat(NewScore));
		ScoreText->SetText(FText::FromString(tempString));

	}
}
