// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "SPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "SGameState.h"

ASPlayerState::ASPlayerState()
{
	PlayerKills = 0;
	PlayerDeaths = 0;
}

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

void ASPlayerState::AddKill()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerKills++;

		// RepNotify callback won't get called by net code if we are the server
		ENetMode NetMode = GetNetMode();
		if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
		{
			OnRep_PlayerKills();
		}
	}
}

void ASPlayerState::AddDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		PlayerDeaths++;

		// RepNotify callback won't get called by net code if we are the server
		ENetMode NetMode = GetNetMode();
		if (NetMode == NM_Standalone || NetMode == NM_ListenServer)
		{
			OnRep_PlayerDeaths();
		}
	}
}

void ASPlayerState::OnRep_PlayerName()
{
	Super::OnRep_PlayerName();
}


void ASPlayerState::OnRep_PlayerKills()
{
	// This is each client's player controller
	ASPlayerController* PC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->UpdatePlayerKillsHUD(PlayerId, PlayerKills);
	}
}

void ASPlayerState::OnRep_PlayerDeaths()
{
	// This is each client's player controller
	ASPlayerController* PC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->UpdatePlayerDeathsHUD(PlayerId, PlayerDeaths);
	}
}

// All clients pickup on this function
// Listen server does not run this code naturally
void ASPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	// This is each client's player controller
	ASPlayerController* PC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		PC->UpdatePlayerScoreHUD(PlayerId, Score);
	}
}

// GameState not valid yet
void ASPlayerState::ClientInitialize(class AController* C)
{
	Super::ClientInitialize(C);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// This replicates to all clients
	DOREPLIFETIME(ASPlayerState, PlayerNumber);
	DOREPLIFETIME(ASPlayerState, PlayerKills);
	DOREPLIFETIME(ASPlayerState, PlayerDeaths);
}