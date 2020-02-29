// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SGameMode.generated.h"

enum class EWaveState : uint8;
class AController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);  // Killed actor, Killer actor,

/**
 * 
 */
UCLASS(config = Game)
class COOPGAME_API ASGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	// Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Start spawning bots
	void StartWave();

	// Stop spawning bots
	void EndWave();

	// Set timer for next startwave
	void PrepareForNextWave();


	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

	void SetWaveState(EWaveState NewState);

	void RestartDeadPlayers();

	UFUNCTION()
	void HandleActorKilled(class AActor* VictimActor, class AActor* KillerActor, class AController* KillerController);

	/** called before startmatch */
	virtual void HandleMatchIsWaitingToStart() override;

	/** starts new match */
	virtual void HandleMatchHasStarted() override;

protected:

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	int32 NrOfBotsToSpawn;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float ScorePerKill;

public:
	ASGameMode();

	virtual void BeginPlay() override;

	virtual void StartPlay() override;

	/**
	 * Accept or reject a player attempting to join the server.  Fails login if you set the ErrorMessage to a non-empty string.
	 * PreLogin is called before Login.  Significant game time may pass before Login is called
	 *
	 * @param	Options					The URL options (e.g. name/spectator) the player has passed
	 * @param	Address					The network address of the player
	 * @param	UniqueId				The unique id the player has passed to the server
	 * @param	ErrorMessage			When set to a non-empty value, the player will be rejected using the error message set
	 */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	/**
 * Called to login new players by creating a player controller, overridable by the game
 *
 * Sets up basic properties of the player (name, unique id, registers with backend, etc) and should not be used to do
 * more complicated game logic.  The player controller is not fully initialized within this function as far as networking is concerned.
 * Save "game logic" for PostLogin which is called shortly afterward.
 *
 * @param NewPlayer pointer to the UPlayer object that represents this player (either local or remote)
 * @param RemoteRole the remote role this controller has
 * @param Portal desired portal location specified by the client
 * @param Options game options passed in by the client at login
 * @param UniqueId platform specific unique identifier for the logging in player
 * @param ErrorMessage [out] error message, if any, why this login will be failing
 *
 * If login is successful, returns a new PlayerController to associate with this player. Login fails if ErrorMessage string is set.
 *
 * @return a new player controller for the logged in player, NULL if login failed for any reason
 */
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	/** Called after a successful login.  This is the first place it is safe to call replicated functions on the PlayerController. */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Event for blueprint to hook on when actor gets killed
	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 NrOfBotsThisWave;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	int32 WaveCount;

};
