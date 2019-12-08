// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"

void USUserWidgetGameInfo::ResetOldInventorySlot()
{
	if (!CurrentOverlay) { return; }
	
	//UE_LOG(LogTemp, Warning, TEXT("found current overlay"));
	if (UBorder* TempBorder = Cast<UBorder>(CurrentOverlay->GetChildAt(0)))
	{
		//UE_LOG(LogTemp, Warning, TEXT("found current Border));
		TempBorder->SetBrushColor(FColor::White);
	}
	CurrentOverlay->SetRenderTranslation(FVector2D(0.f, 0.f));
}

void USUserWidgetGameInfo::UpdateNewInventorySlot(UOverlay* NewOverlay)
{
	if (!NewOverlay) { return; }

	if (UBorder* TempBorder = Cast<UBorder>(NewOverlay->GetChildAt(0)))
	{
		TempBorder->SetBrushColor(FColor::Blue);
		NewOverlay->SetRenderTranslation(FVector2D(0.f, -10.f));
		CurrentOverlay = NewOverlay;
	}
}

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
		FString tempString("SCORE: " + FString::SanitizeFloat(NewScore));
		ScoreText->SetText(FText::FromString(tempString));

	}
}

void USUserWidgetGameInfo::SetInventoryColor(int WeaponSlot)
{
	switch (WeaponSlot)
	{
		case 0:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FirstOverlay);
			break;

		case 1:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(SecondOverlay);
			break;
		case 2:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(ThirdOverlay);
			break;
		case 3:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FourthOverlay);
			break;
		case 4:
			ResetOldInventorySlot();
			UpdateNewInventorySlot(FifthOverlay);
			break;
	default:
		break;
	}
}
