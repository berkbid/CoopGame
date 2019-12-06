// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SCharacter.h"
#include "SPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API ASPlayerCharacter : public ASCharacter
{
	GENERATED_BODY()
	


public:
	ASPlayerCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetPawnViewLocation() const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComp;

	void MoveForward(float Amount);

	void MoveRight(float Amount);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	void EquipSlotOne();
	void EquipSlotTwo();
	void EquipSlotThree();
	void EquipSlotFour();
	void EquipSlotFive();

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

	// This will only run on server instead of client, and reliable so will eventually get to server, need reliable since gameplay critical component
	// WithValidation is required for something
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerChangeWeapons(TSubclassOf<class ASWeapon> NewWeaponClass, int NewWeaponSlot);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
