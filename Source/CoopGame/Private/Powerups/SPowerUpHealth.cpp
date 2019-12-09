// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpHealth.h"
#include "Kismet/GameplayStatics.h"
#include "SCharacter.h"
#include "SHealthComponent.h"

ASPowerUpHealth::ASPowerUpHealth()
{
	HealAmount = 25.f;
}

void ASPowerUpHealth::OnTickPowerup()
{
	TicksSoFar++;

	if (PowerupTargetActor)
	{
		USHealthComponent* HealthComp = Cast<USHealthComponent>(PowerupTargetActor->GetComponentByClass(HealthComponentClass));
		if (HealthComp)
		{
			HealthComp->Heal(HealAmount);
		}
	}

	if (TicksSoFar >= TotalNrOfTicks)
	{
		Destroy();
	}
}
