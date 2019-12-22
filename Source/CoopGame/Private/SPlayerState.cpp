// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


// Allows blueprint to manipulate "Score" since it is marked BlueprintReadOnly in PlayerState.h
void ASPlayerState::AddScore(float ScoreDelta)
{
	if (GetLocalRole() == ROLE_Authority)
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

	//UE_LOG(LogTemp, Warning, TEXT("FOUND Name: %s"), *GetPlayerName());
}


// All clients pickup on this function
// Listen server does not run this code naturally
void ASPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
	// We want to update everyone's scoreboard HUD



	//ASPlayerController* PCC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	//if (PCC)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("FOUND Controller: %s"), *PCC->GetName());
	//}

	// Owner of playerstate is of type playercontroller, this is only valid on owning client
	ASPlayerController* PC = Cast<ASPlayerController>(GetOwner());
	if (PC)
	{
		PC->SetScoreText(Score);
	}
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// This replicates to all clients
	DOREPLIFETIME(ASPlayerState, PlayerNumber);
	DOREPLIFETIME(ASPlayerState, PlayerKills);
	DOREPLIFETIME(ASPlayerState, PlayerDeaths);
}
