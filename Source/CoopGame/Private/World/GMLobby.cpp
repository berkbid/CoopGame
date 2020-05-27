// Fill out your copyright notice in the Description page of Project Settings.


#include "GMLobby.h"
#include "LobbyGameSession.h"
#include "PCLobby.h"

AGMLobby::AGMLobby()
{
	// Set default GameSession class to use to our custom LobbyGameSession
	TSubclassOf<ALobbyGameSession> GameSClass = ALobbyGameSession::StaticClass();
	if (GameSClass)
	{
		GameSessionClass = GameSClass;
		UE_LOG(LogTemp, Warning, TEXT("GMLobby found GameSession Class"));
	}

	// Set default PlayerController class to use our custom PCLobby
	TSubclassOf<APCLobby> PCClass = APCLobby::StaticClass();
	if (PCClass)
	{
		PlayerControllerClass = PCClass;
		UE_LOG(LogTemp, Warning, TEXT("GMLobby found PCLobby Class"));
	}

	// Make sure we use seamless travel, since all players are already connected?
	bUseSeamlessTravel = true;
}

void AGMLobby::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("GMLobby: New Player Joined Lobby!: %s"), *NewPlayer->GetName());
	}
	
	
}

void AGMLobby::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (Exiting)
	{
		UE_LOG(LogTemp, Warning, TEXT("GMLobby: Player Leaving Lobby!: %s"), *Exiting->GetName());
	}
	
}

void AGMLobby::TimeToGoToGame()
{

}
