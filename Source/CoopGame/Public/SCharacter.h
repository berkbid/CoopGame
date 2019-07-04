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

	class ASWeapon* CurrentWeapon;

	// Weapon that we spawn with
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	void Fire();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

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
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;


};
