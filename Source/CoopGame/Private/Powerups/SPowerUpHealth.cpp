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

	// If want to heal all actors can get them with this
	//TArray<AActor*> CharArray;
	//UGameplayStatics::GetAllActorsOfClass(this, CharClass, CharArray);

	if (PowerupTargetActor)
	{
		USHealthComponent* HealthComp = Cast<USHealthComponent>(PowerupTargetActor->GetComponentByClass(HealthComponentClass));
		if (HealthComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("HEALING"));
			HealthComp->Heal(HealAmount);
		}
	}

	if (TicksSoFar >= TotalNrOfTicks)
	{
		Destroy();
	}
}
