// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGrenadeProjectile.generated.h"

UCLASS()
class COOPGAME_API ASGrenadeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASGrenadeProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	class URadialForceComponent* RadialForceComp;

	FTimerHandle TimerHandle_TimeUntilExplode;

	void Explode();

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	class UParticleSystem* ExplosionEffect;

	UFUNCTION()
	void MyOnDestroyed(AActor* DestroyedActor);

public:	


};
