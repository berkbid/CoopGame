// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMode.h"
#include "Public/TimerManager.h"
#include "Engine/World.h"
#include "SHealthComponent.h"
#include "STrackerBot.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "SPlayerCharacter.h"
#include "SAICharacter.h"

ASGameMode::ASGameMode()
{
	TimeBetweenWaves = 2.f;
	NrOfBotsToSpawn = 2;

	// Setup default gamestate class
	GameStateClass = ASGameState::StaticClass();
	PlayerStateClass = ASPlayerState::StaticClass();
	
}


void ASGameMode::BeginPlay()
{
	Super::BeginPlay();

	OnActorKilled.AddDynamic(this, &ASGameMode::HandleActorKilled);
}

void ASGameMode::StartWave()
{
	WaveCount++;
	NrOfBotsThisWave = NrOfBotsToSpawn * WaveCount;

	//UE_LOG(LogTemp, Warning, TEXT("StartWave(), nrbots: %d"), NrOfBotsThisWave);
	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &ASGameMode::SpawnBotTimerElapsed, 1.f, true, 0.f);


	SetWaveState(EWaveState::WaveInProgress);
}

void ASGameMode::SpawnBotTimerElapsed()
{
	// Blueprint function
	SpawnNewBot();

	NrOfBotsThisWave--;

	//UE_LOG(LogTemp, Warning, TEXT("NrOfBotsThisWave="));
	if (NrOfBotsThisWave <= 0)
	{
		EndWave();
	}
}

void ASGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	//PrepareForNextWave();

	SetWaveState(EWaveState::WaitingToComplete);
}

void ASGameMode::PrepareForNextWave()
{
	//UE_LOG(LogTemp, Warning, TEXT("PrepareForNextWave()"))
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &ASGameMode::StartWave, TimeBetweenWaves, false);

	SetWaveState(EWaveState::WaitingToStart);

	RestartDeadPlayers();
}

void ASGameMode::CheckWaveState()
{
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	// Never want to prepare for next wave if already preparing or if still have bots to spawn
	if (NrOfBotsThisWave > 0 ||  bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}
		
		USHealthComponent* HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		SetWaveState(EWaveState::WaveComplete);
		PrepareForNextWave();
	}

}

void ASGameMode::CheckAnyPlayerAlive()
{

	//UE_LOG(LogTemp, Warning, TEXT("CHECKING PLAYERS ALIVE"));
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (PC && PC->GetPawn())
		{
			APawn* MyPawn = PC->GetPawn();
			USHealthComponent* HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
			// Code will break if check returns false, player has to have health component so we can ensure this
			if (ensure(HealthComp) && HealthComp->GetHealth() > 0.f)
			{
				// A player is still alive
				return;
			}
		}
	}
	
	// No player alive
	GameOver();


}

void ASGameMode::GameOver()
{
	EndWave();

	SetWaveState(EWaveState::GameOver);
	// @TODO: Finish up the match, present 'game over' to players.
}

void ASGameMode::SetWaveState(EWaveState NewState)
{
	ASGameState* GS = GetGameState<ASGameState>();
	// Need to be notified if this check doesnt succeed, "ensureAlways"
	if (GS)
	{
		// This method in GS will handle replicating new wave state for server and clients
		GS->SetWaveState(NewState);
		
	}
}

void ASGameMode::RestartDeadPlayers()
{
	// Respawn everyone not alive, iterate over server player controllers, ALL PLAYER CONTROLLERS ARE AVAILABLE
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		// Only need to check if doesnt have pawn
		if (PC && (PC->GetPawn() == nullptr))
		{
			RestartPlayer(PC);
		}
	}
}


void ASGameMode::HandleActorKilled(AActor* VictimActor, AActor* KillerActor, AController* KillerController)
{
	// If the killing actor is a player, add score
	ASPlayerCharacter* KillerPawn = Cast<ASPlayerCharacter>(KillerActor);
	if (KillerPawn)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(KillerPawn->GetPlayerState());
		if (PS)
		{
			PS->AddScore(20.f);
		}
	}

	
	ASPlayerCharacter* VictimPawn = Cast<ASPlayerCharacter>(VictimActor);
	// If victim is player, check if any player is alive
	if (VictimPawn)
	{
		CheckAnyPlayerAlive();
	}
	// If victim is a bot, check wave state
	else
	{
		CheckWaveState();
	}
}

// Overriden from GameStateBase
void ASGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();

}
