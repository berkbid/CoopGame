// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// Only local controllers can add widgets
	//if (Role < ROLE_Authority)
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (wGameInfo)
		{
			MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);

			if (MyGameInfo)
			{
				MyGameInfo->SetOwningController(this);
				MyGameInfo->AddToViewport();
				
			}
		}
	}
}

void ASPlayerController::SetScoreText(float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->HandleScoreChanged(NewScore);
	}
}