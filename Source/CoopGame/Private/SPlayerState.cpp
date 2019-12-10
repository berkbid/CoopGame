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
		
		// RepNotify callback won't get called by net code if we are the server
		ENetMode NetMode = GetNetMode();
		if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
		{
			OnRep_Score();
		}
	}

}

void ASPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();

	//UE_LOG(LogTemp, Warning, TEXT("FOUND Name: %s"), *PlayerName);
}
// All clients pickup on this function
// Listen server does not run this code
void ASPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	
	// Owner if playerstate is of type playercontroller, this is only valid on owning client
	ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());
	if (PC)
	{
		//UE_LOG(LogTemp, Warning, TEXT("FOUND OWNER: %s, Score: %f"), *PC->GetName(), Score);
		PC->SetScoreText(Score);
	}

}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	// This replicates to any client connected to us
	DOREPLIFETIME(ASPlayerState, PlayerNumber);
	DOREPLIFETIME(ASPlayerState, PlayerKills);
	DOREPLIFETIME(ASPlayerState, PlayerDeaths);
}
