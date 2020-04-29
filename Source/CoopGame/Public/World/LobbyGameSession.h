// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "LobbyGameSession.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ALobbyGameSession : public AGameSession
{
	GENERATED_BODY()
	

protected:

	/**
	 * Called by GameMode::PostLogin to give session code chance to do work after PostLogin
	 *
	 * @param NewPlayer player logging in
	 */
	void PostLogin(APlayerController* NewPlayer) override;

	//=================================================================================
	// LOGOUT

	/**
	* called after a seamless level transition has been completed on the *new* GameMode
	* used to reinitialize players already in the game as they won't have *Login() called on them
	*/
	void PostSeamlessTravel() override;

	//=================================================================================
	// MATCH INTERFACE

	/** @RETURNS true if GameSession handled the request, in case it wants to stall for some reason. Otherwise, game mode will start immediately */
	bool HandleStartMatchRequest() override;

	/** Handle when the match enters waiting to start */
	void HandleMatchIsWaitingToStart() override;

	/** Handle when the match has started */
	void HandleMatchHasStarted() override;

	/** Handle when the match has completed */
	void HandleMatchHasEnded() override;

	/** Called from GameMode.RestartGame(). */
	bool CanRestartGame() override;

private:

	/**
	 * Delegate called when StartSession has completed
	 *
	 * @param InSessionName name of session involved
	 * @param bWasSuccessful true if the call was successful, false otherwise
	 */
	void OnStartSessionComplete(FName InSessionName, bool bWasSuccessful) override;

	/**
		* Delegate called when EndSession has completed
		*
		* @param InSessionName name of session involved
		* @param bWasSuccessful true if the call was successful, false otherwise
		*/
	void OnEndSessionComplete(FName InSessionName, bool bWasSuccessful) override;
};
