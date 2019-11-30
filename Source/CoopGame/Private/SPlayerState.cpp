// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SPlayerController.h"


// Allows blueprint to manipulate "Score" since it is marked BlueprintReadOnly in PlayerState.h
void ASPlayerState::AddScore(float ScoreDelta)
{
	if (Role == ROLE_Authority)
	{
		Score += ScoreDelta;
		//UE_LOG(LogTemp, Warning, TEXT("New SCore: %f"), Score);

		//ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());
		//if (PC)
		//{
		//	PC->SetScoreText(Score);
		//}

		//OnScoreChanged.Broadcast(Score);
	}

}
