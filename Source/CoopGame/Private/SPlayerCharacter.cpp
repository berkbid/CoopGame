// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "SWeapon.h"


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
}

// Called once on client and once on server
// Called when the game starts or when spawned
void ASPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Should this not be run on server
	DefaultFOV = CameraComp->FieldOfView;

}

// Called every frame
void ASPlayerCharacter::Tick(float DeltaTime)
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

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ASPlayerCharacter::EquipSlotOne);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ASPlayerCharacter::EquipSlotTwo);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ASPlayerCharacter::EquipSlotThree);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ASPlayerCharacter::EquipSlotFour);
	PlayerInputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerCharacter::EquipSlotFive);
}

FVector ASPlayerCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
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

void ASPlayerCharacter::EquipSlotOne()
{
	if (!FirstWeaponClass || (CurrentWeapon->GetClass() == FirstWeaponClass)) { return; }
	ServerChangeWeapons(FirstWeaponClass);
}

void ASPlayerCharacter::EquipSlotTwo()
{
	if (!SecondWeaponClass || (CurrentWeapon->GetClass() == SecondWeaponClass)) { return; }
	ServerChangeWeapons(SecondWeaponClass);
}

void ASPlayerCharacter::EquipSlotThree()
{
	if (!ThirdWeaponClass || (CurrentWeapon->GetClass() == ThirdWeaponClass)) { return; }
	ServerChangeWeapons(ThirdWeaponClass);
}

void ASPlayerCharacter::EquipSlotFour()
{
	if (!FourthWeaponClass || (CurrentWeapon->GetClass() == FourthWeaponClass)) { return; }
	ServerChangeWeapons(FourthWeaponClass);
}

void ASPlayerCharacter::EquipSlotFive()
{
	if (!FifthWeaponClass || (CurrentWeapon->GetClass() == FifthWeaponClass)) { return; }
	ServerChangeWeapons(FifthWeaponClass);
}

// MUST prefix with Server and require _Implementation
void ASPlayerCharacter::ServerChangeWeapons_Implementation(TSubclassOf<ASWeapon> NewWeaponClass)
{
	ChangeWeapons(NewWeaponClass);
}

bool ASPlayerCharacter::ServerChangeWeapons_Validate(TSubclassOf<ASWeapon> NewWeaponClass)
{
	// This is for anti cheat stuff
	return true;
}

void ASPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}