// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameSession.h"

void ALobbyGameSession::PostLogin(APlayerController* NewPlayer)
{
	// Super() implementation is empty
}

void ALobbyGameSession::PostSeamlessTravel()
{
	// Super() implementation is empty
}

bool ALobbyGameSession::HandleStartMatchRequest()
{
	// Returns false by default
	return Super::HandleStartMatchRequest();
}

void ALobbyGameSession::HandleMatchIsWaitingToStart()
{
	// Super() implementation is empty
}

void ALobbyGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void ALobbyGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}

bool ALobbyGameSession::CanRestartGame()
{
	// Returns true by default
	return Super::CanRestartGame();
}

void ALobbyGameSession::OnStartSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	// Super() implementation is empty
}

void ALobbyGameSession::OnEndSessionComplete(FName InSessionName, bool bWasSuccessful)
{
	// Super() implementation is empty
}