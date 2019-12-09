// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpSpeed.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Public/TimerManager.h"
#include "Net/UnrealNetwork.h"


void ASPowerUpSpeed::OnRep_SpeedChange()
{
	//UE_LOG(LogTemp, Warning, TEXT("OnRep Called"));
	if (MoveComp)
	{
		//UE_LOG(LogTemp, Warning, TEXT("MoveComp Found"));
		MoveComp->MaxWalkSpeed = NewMaxSpeed;
	}
}


void ASPowerUpSpeed::OnSpeedTick()
{
	TicksSoFar++;

	// Undo previous increase to CharacterMovementComponent
	if (TicksSoFar >= TotalNrOfTicks)
	{
		if (PowerupTargetActor)
		{
			NewMaxSpeed = DefaultMaxWalkSpeed;
			OnRep_SpeedChange();

		}

		// This is called too fast that clients dont update the movement component
		//Destroy();
		SetLifeSpan(1.f);
	}
	else
	{
		// Try to activate powerup on actor's CharacterMovementComponent
		if (PowerupTargetActor)
		{
			if (MoveComp)
			{
				// Need to replicate this code
				DefaultMaxWalkSpeed = MoveComp->MaxWalkSpeed;

				NewMaxSpeed = DefaultMaxWalkSpeed * 2;
				OnRep_SpeedChange();
			}
		}
	}
}

// Server runs this code
void ASPowerUpSpeed::ActivatePowerup(AActor* PowerActor)
{
	PowerupTargetActor = PowerActor;

	// Trigger replication for clients, setting actor invisible
	bIsPowerupActive = true;
	// Call function manually for server
	OnRep_PowerupActive();

	// Using PowerActor instead of PowerupTargetActor to get the component
	// Server updates this variable, that is why it is replicated, so when clients enter OnRep_SpeedChange, they have a valid copy
	// of MoveComp in order to run the replicated code
	MoveComp = Cast<UCharacterMovementComponent>(PowerActor->GetComponentByClass(UCharacterMovementComponent::StaticClass()));

	if (PowerupInterval > 0.f)
	{
		// Calls parent method OnTickPowerup which is overriden above, though could just make new method in this class and call it
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerUpSpeed::OnSpeedTick, PowerupInterval, true, 0.f);
	}
	else
	{
		OnTickPowerup();
	}
}

void ASPowerUpSpeed::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	// Use condition b/c do not want to replicate it to client who owns this weapon
	// do not want to play visual effects twice
	DOREPLIFETIME(ASPowerUpSpeed, MoveComp);
	DOREPLIFETIME(ASPowerUpSpeed, NewMaxSpeed);
}
