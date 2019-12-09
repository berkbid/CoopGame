// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"


// Allows blueprint to manipulate "Score" since it is marked BlueprintReadOnly in PlayerState.h
void ASPlayerState::AddScore(float ScoreDelta)
{
	if (Role == ROLE_Authority)
	{
		Score += ScoreDelta;
	}

}

// All clients pickup on this function
// Listen server does not run this code
void ASPlayerState::OnRep_Score()
{
	//UE_LOG(LogTemp, Warning, TEXT("Name: %s, Controller: %s, New Score: %f"),*GetPawn()->GetName(), *GetOwner()->GetName(), Score);

	// each client update their own HUD for this playerstate's entry!
	
	
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// This replicates to any client connected to us
	DOREPLIFETIME(ASPlayerState, PlayerNumber);
	DOREPLIFETIME(ASPlayerState, PlayerKills);
	DOREPLIFETIME(ASPlayerState, PlayerDeaths);
}
