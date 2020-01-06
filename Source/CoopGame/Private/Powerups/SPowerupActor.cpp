// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerupActor.h"
#include "Public/TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Components/PointLightComponent.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	RootComponent = MeshComp;

	LightComp = CreateDefaultSubobject<UPointLightComponent>("LightComp");
	LightComp->CastShadows = false;
	LightComp->SetupAttachment(RootComponent);

	RotatingComp = CreateDefaultSubobject<URotatingMovementComponent>("RotatingComp");

	PowerupInterval = 0.f;
	TotalNrOfTicks = 0.f;
	PowerupTargetActor = nullptr;

	bIsPowerupActive = false;

	SetReplicates(true);
}



void ASPowerupActor::OnTickPowerup()
{
	TicksSoFar++;

	if (TicksSoFar >= TotalNrOfTicks)
	{
		Destroy();
	}
}

void ASPowerupActor::OnRep_PowerupActive()
{
	//OnPowerupStateChanged(bIsPowerupActive);
	// WRite this code here instead!
	if (bIsPowerupActive)
	{
		// Propogate setting visibility to child (PointLight) also !
		MeshComp->SetVisibility(false, true);
	}

}

void ASPowerupActor::ActivatePowerup(AActor* PowerActor)
{
	PowerupTargetActor = PowerActor;

	// Trigger replication for clients
	bIsPowerupActive = true;
	// Call function manually for server
	OnRep_PowerupActive();

	if (PowerupInterval > 0.f)
	{
		// Ticks for first time after interval 
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true, 0.f);
	}
	else
	{
		OnTickPowerup();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	// Use condition b/c do not want to replicate it to client who owns this weapon
	// do not want to play visual effects twice
	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}