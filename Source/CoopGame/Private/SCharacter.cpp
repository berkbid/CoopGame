// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "CoopGame.h"
#include "SHealthComponent.h"
#include "SWeapon.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentWeapon = nullptr;
	WeaponAttachSocketName = "WeaponSocket";

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	// We do not want capsule to block the weapon trace, only the mesh of character
	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	// Needed to make sure we are allowed to crouch
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	HealthComp = CreateDefaultSubobject<USHealthComponent>("HealthComp");

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	bDied = false;
	bIsZoomingIn = false;
	bIsZoomingOut = false;
	bIsZooming = false;
	ZoomedFOV = 65.f;
	ZoomInterpSpeed = 20.f;

}

// Called once on client and once on server
// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	// Only runs this code on server
	// Spawn weapon on server, it is set to replicated in SWeapon.cpp thus will exist on clients as well
	if (Role == ROLE_Authority)
	{
		// Spawn a default weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// Need client to have this "CurrentWeapon" variable set also to call StartFire() and StopFire()
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}

}

void ASCharacter::MoveForward(float Amount)
{
	AddMovementInput(GetActorForwardVector(), Amount);
}

void ASCharacter::MoveRight(float Amount)
{
	AddMovementInput(GetActorRightVector(), Amount);
}

void ASCharacter::BeginCrouch()
{
	//UE_LOG(LogTemp, Warning, TEXT("Crouching"));
	Crouch();
}

void ASCharacter::EndCrouch()
{
	//UE_LOG(LogTemp, Warning, TEXT("Un-Crouching"));
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bIsZooming = true;
	bIsZoomingOut = false;
	bIsZoomingIn = true;
}

void ASCharacter::EndZoom()
{
	bIsZooming = true;
	bIsZoomingIn = false;
	bIsZoomingOut = true;
}

void ASCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}

// Only called on server because we only hooked this on the server
void ASCharacter::OnHealthChanged(USHealthComponent* HealthCompNew, float Health, float HealthDelt, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	// This was previously never run on the client because bDied replicates the same time HealthChanged event triggers on the client
	if (Health <= 0.f && !bDied)
	{
		bDied = true;
		// This will manually call this method for the server, meanwhile clients call the method when bDied gets changed & ReplicatedUsing=OnRepDeath()
		OnRep_Death();
	}
}

void ASCharacter::OnRep_Death()
{
	// DIE!
	
	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	// Detaches player from the actor, then actor destroys in 10 seconds
	DetachFromControllerPendingDestroy();
	SetLifeSpan(10.f);
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		//UE_LOG(LogTemp, Warning, TEXT("NewFOV = %f"), NewFOV);
		CameraComp->SetFieldOfView(NewFOV);
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);
	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::StopFire);
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Specify what we want to replicate and how we want to replicate it
	// In .h file we say we want to replicate CurrentWeapon variable, now we specify where we want to replicate to
	// This replicates to any client connected to us
	DOREPLIFETIME(ASCharacter, CurrentWeapon);
	DOREPLIFETIME(ASCharacter, bDied);
}
