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

	TraceObjectQueryParams.AddObjectTypesToQuery(COLLISION_INTERACTABLEOBJECT);
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
	if (CurrentWeapon)
	{
		USoundBase* SB = CurrentWeapon->WeaponSwapSound;
		if (SB)
		{
			UGameplayStatics::PlaySoundAtLocation(this, SB, GetActorLocation());
		}
	}
}

void ASPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Only want listen server or owning client to run this code
	if (IsLocallyControlled())
	{
		HandleZoom(DeltaTime);
		TraceForInteractables();
	}
}

void ASPlayerCharacter::TraceForInteractables()
{
	FVector EyeLocation;
	FRotator EyeRotation;
	// We override the location return in SCharacter.cpp to return camera location instead
	GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector TraceDirection = EyeRotation.Vector();
	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + (TraceDirection * 450.f);
	//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, .1f, 0, 5.f);
	
	TArray<FHitResult> HitArray;

	if (GetWorld()->LineTraceMultiByObjectType(HitArray, TraceStart, TraceEnd, TraceObjectQueryParams))
	{
		AActor* HitActor = HitArray.Last().GetActor();
		if (!HitActor) 
		{ 
			// If we were previously interacting with an object, stop interacting with it
			if (CurrentSelectedInteractable)
			{
				CurrentSelectedInteractable->ShowItemInfo(false);
				CurrentSelectedInteractable = nullptr;
			}
			return; 
		}
		ASInteractable* HitInteractable = Cast<ASInteractable>(HitActor);
		if (!HitInteractable) 
		{ 
			// If we were previously interacting with an object, stop interacting with it
			if (CurrentSelectedInteractable)
			{
				CurrentSelectedInteractable->ShowItemInfo(false);
				CurrentSelectedInteractable = nullptr;
			}
			return; 
		}

		// If we were previously interacting with an object
		if (CurrentSelectedInteractable)
		{
			// And the new object is a different object
			if (HitInteractable != CurrentSelectedInteractable)
			{
				CurrentSelectedInteractable->ShowItemInfo(false);
				HitInteractable->ShowItemInfo(true);
				CurrentSelectedInteractable = HitInteractable;
			}
		}
		// If we find a new interactable and were previously interacting with nothing
		else
		{
			HitInteractable->ShowItemInfo(true);
			CurrentSelectedInteractable = HitInteractable;
		}
	}
	// If we didn't hit anything, update item info showing if necessary
	else
	{
		// If we were previously interacting with an object, stop interacting with it
		if (CurrentSelectedInteractable)
		{
			CurrentSelectedInteractable->ShowItemInfo(false);
			CurrentSelectedInteractable = nullptr;
		}
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
	if (bIsZooming)
	{
		// This will get set in either the if or else statement below
		float NewFOV;

		// If we are zooming in
		if (bIsZoomingIn)
		{
			NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, ZoomedFOV, DeltaTime, ZoomInterpSpeed);
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
			if (NewFOV == DefaultFOV)
			{
				bIsZoomingOut = false;
				bIsZooming = false;
			}
		}
		CameraComp->SetFieldOfView(NewFOV);
	}
}

void ASPlayerCharacter::BeginZoom()
{
	bIsZooming = true;
	bIsZoomingOut = false;
	bIsZoomingIn = true;
}

void ASPlayerCharacter::EndZoom()
{
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