// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SRifle.generated.h"


// Contains information of a single hitscan weapon linetrace
USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;

	// Used to update struct on a line trace to the SAME TraceTo value
	UPROPERTY()
	uint8 ReplicationCount;
};

/**
 * 
 */
UCLASS()
class COOPGAME_API ASRifle : public ASWeapon
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	ASRifle();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void Fire() override;

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	void PlayFireEffect(FVector TracerEndPoint);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	class UParticleSystem* TracerEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<class UCameraShake> FireCamShake;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Effects")
	FName TracerTargetName;

	/* Bullet Spread in Degrees */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (ClampMin = 0.f))
	float BulletSpread;

	// Trigger function OnRep_HitScanTrace every time property is replicated(changed) because "ReplicatedUsing=..."
	// Thus, whenever we shoot on server, need to update struct for clients to trigger this function and play effects
	UPROPERTY(ReplicatedUsing = OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
};
