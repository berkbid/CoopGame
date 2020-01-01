// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameState.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

void ASGameState::OnRep_WaveState(EWaveState OldState)
{
	// We capture old state value above in replication technique
	//WaveStateChanged(WaveState, OldState);

	UpdateWaveStateHUD();

}


void ASGameState::SetWaveState(EWaveState NewState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EWaveState OldState = WaveState;

		WaveState = NewState;

		// Call code manually on server
		OnRep_WaveState(OldState);
	}
}

void ASGameState::UpdateWaveStateHUD()
{
	ASPlayerController* PC = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PC)
	{
		FString WaveStateString = GetWaveStateString();
		PC->SetStateText(WaveStateString);
	}
}

FString ASGameState::GetWaveStateString()
{
	FString StateString;

	switch (WaveState)
	{
	case EWaveState::WaitingToStart:
		StateString = "Waiting To Start";
		break;
	case EWaveState::WaveInProgress:
		StateString = "Wave Spawning";
		break;
	case EWaveState::WaitingToComplete:
		StateString = "Waiting To Complete";
		break;
	case EWaveState::GameOver:
		StateString = "Game Over";
		break;
	case EWaveState::WaveComplete:
		StateString = "Wave Complete";
		break;
	default:
		break;
	}
	return StateString;
}

void ASGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);


	DOREPLIFETIME(ASGameState, WaveState);
}
