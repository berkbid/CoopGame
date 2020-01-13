// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "SWeapon.h"
#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SWidgetCompHealthBar.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "CoopGame.h"
#include "DrawDebugHelpers.h"
#include "SInteractable.h"


ASPlayerCharacter::ASPlayerCharacter()
{
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	bIsZoomingIn = false;
	bIsZoomingOut = false;
	bIsZooming = false;
	ZoomedFOV = 65.f;
	ZoomInterpSpeed = 20.f;

	NetUpdateFrequency = 144.f;
	MinNetUpdateFrequency = 33.f;
	NetPriority = 1.f;
}

// Called once on client and once on server
void ASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		DefaultFOV = CameraComp->FieldOfView;
	}
}

// Handle when ASWeapon* CurrentWeapon changes value
void ASPlayerCharacter::OnRep_CurrentWeapon()
{
	Super::OnRep_CurrentWeapon();

	// This checks if we changed to an actual weapon
	if (!CurrentWeapon) { return; }
	
	USoundBase* SB = CurrentWeapon->WeaponSwapSound;
	if (SB)
	{
		UGameplayStatics::PlaySound2D(this, SB);
	}
}

void ASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only want listen server or owning client to run this code
	if (IsLocallyControlled())
	{
		HandleZoom(DeltaTime);
	}
}

// Called to bind functionality to input
void ASPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASPlayerCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASPlayerCharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASPlayerCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASPlayerCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASPlayerCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASPlayerCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASPlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASPlayerCharacter::StopFire);

	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASPlayerCharacter::Reload);

}

void ASPlayerCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector(), Amount);
}

void ASPlayerCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector(), Amount);
}

void ASPlayerCharacter::BeginCrouch()
{
	Crouch();
}

void ASPlayerCharacter::EndCrouch()
{
	UnCrouch();
}

void ASPlayerCharacter::HandleZoom(float DeltaTime)
{
	// If we are either currently zooming in or out this is true, otherwise do not enter this code during tick function
	if (!bIsZooming) { return; }
	
	// Calculate new FOV we want to set camera to
	float NewFOV; 
	
	// If we are zooming in
	if (bIsZoomingIn)
	{	
		NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, ZoomedFOV, DeltaTime, ZoomInterpSpeed);
		// And our new FOV equals our desired zoomed FOV, we are no longer zooming in or zooming
		if (NewFOV == ZoomedFOV)
		{
			bIsZoomingIn = false;
			bIsZooming = false;
		}
	}
	// If we are zooming out
	else
	{
		NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, DefaultFOV, DeltaTime, ZoomInterpSpeed);
		// And our new FOV equals our default FOV we are no longer zooming out or zooming
		if (NewFOV == DefaultFOV)
		{
			bIsZoomingOut = false;
			bIsZooming = false;
		}
	}
	CameraComp->SetFieldOfView(NewFOV);
	
}

void ASPlayerCharacter::BeginZoom()
{
	// Setup the state of zooming and zooming in and not zooming out
	bIsZooming = true;
	bIsZoomingOut = false;
	bIsZoomingIn = true;
}

void ASPlayerCharacter::EndZoom()
{
	// Setup the zoom state where we are zooming out and not zooming in, but we are still zooming (out) until we reach default FOV
	bIsZooming = true;
	bIsZoomingIn = false;
	bIsZoomingOut = true;
}

FVector ASPlayerCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}