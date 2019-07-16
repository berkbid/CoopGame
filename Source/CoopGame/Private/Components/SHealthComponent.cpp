// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/WidgetTree.h"
#include "SGameMode.h"
#include "Engine/World.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
	MyOwner = nullptr;
	Health = DefaultHealth;
	bIsDead = false;

	SetIsReplicated(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Only hook event if we are server!!!
	// Components dont have "Role" only actor does, thus GetOwnerRole() is used here
	if (GetOwnerRole() == ROLE_Authority)
	{
		// Now we handle the event of owner taking any damage in this component
		MyOwner = GetOwner();
		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}

}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_Health Called, health = %f, oldhealth = %f"), Health, OldHealth)
	// Last value of Health is saved in OldHealth as a little OnRep trick
	float Damage = OldHealth - Health;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

// Find this signature to override in Actor.h under the DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams( FTakeAnyDamageSignature,...)
void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.f || bIsDead)
	{
		return;
	}

	// Update health clamped
	Health = FMath::Clamp(Health - Damage, 0.f, DefaultHealth);
	UE_LOG(LogTemp, Warning, TEXT("%s Health: %f"), *MyOwner->GetName(), Health);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

	bIsDead = (Health <= 0.f);
	if (bIsDead)
	{
		bIsDead = true;
		// This will only be valid on the server
		ASGameMode* GM = Cast<ASGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
			//if (DamageCauser)
			//{
			//	AActor* DamageCauserOwner = DamageCauser->GetOwner();
			//	if (DamageCauserOwner)
			//	{
			//		//UE_LOG(LogTemp, Warning, TEXT("Victim Actor: %s"), *GetOwner()->GetName());
			//		GM->OnActorKilled.Broadcast(GetOwner(), DamageCauserOwner, InstigatedBy);
			//	}
			//		
			//}
			
		}
	}

}

float USHealthComponent::GetHealth() const
{
	return Health;
}

// Called to add health
void USHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.f || Health <= 0.f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.f, DefaultHealth);
	//UE_LOG(LogTemp, Warning, TEXT("%s HEALED FOR: %f new health: %f"), *MyOwner->GetName(), HealAmount, Health);
	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	// Use condition b/c do not want to replicate it to client who owns this weapon
	// do not want to play visual effects twice
	DOREPLIFETIME(USHealthComponent, Health);

}

