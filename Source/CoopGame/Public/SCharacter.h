// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Amount);

	void MoveRight(float Amount);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	// Replicate this variable for clients to have access as well!
	UPROPERTY(Replicated)
	class ASWeapon* CurrentWeapon;

	// Weapon that we spawn with
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComp;

	bool bIsZooming;
	bool bIsZoomingIn;
	bool bIsZoomingOut;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	/* Default FOV set during begin play*/
	float DefaultFOV;

	// Clamps the values to change in edit from 0.1 to 100
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthCompNew, float Health, float HealthDelt, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	
	// We replicate this boolean so when player dies on server, all clients get updated thus playing replicated death animation
	UPROPERTY(ReplicatedUsing=OnRep_Death, BlueprintReadOnly, Category = "Player")
	bool bDied;

	UFUNCTION()
	void OnRep_Death();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();
};
