// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/InputComponent.h"
#include "SWeapon.h"
#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"

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


	// Only do these things on clients, and listening server client
	if (IsLocallyControlled())
	{
		DefaultFOV = CameraComp->FieldOfView;

		// Check which weapon slots are valid, fill in their images, and set current weapon to fist class if it is valid
		if (ASPlayerController* PC = Cast<ASPlayerController>(GetController()))
		{
			// If we have a weapon equipped, then set the HUD accordingly
			if (FirstWeaponClass)
			{
				PC->SetCurrentWeapon(FirstWeaponClass, 0);
			}
			if (SecondWeaponClass)
			{
				USUserWidgetGameInfo* WGI = PC->MyGameInfo;
				if (WGI)
				{
					WGI->SetInventoryImage(SecondWeaponClass, 1);
				}
			}
			if (ThirdWeaponClass)
			{
				USUserWidgetGameInfo* WGI = PC->MyGameInfo;
				if (WGI)
				{
					WGI->SetInventoryImage(ThirdWeaponClass, 2);
				}
			}
			if (FourthWeaponClass)
			{
				USUserWidgetGameInfo* WGI = PC->MyGameInfo;
				if (WGI)
				{
					WGI->SetInventoryImage(FourthWeaponClass, 3);
				}
			}
			if (FifthWeaponClass)
			{
				USUserWidgetGameInfo* WGI = PC->MyGameInfo;
				if (WGI)
				{
					WGI->SetInventoryImage(FifthWeaponClass, 4);
				}
			}
		}

	}

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
	if (!FirstWeaponClass) { return; }
	ChangeWeapons(FirstWeaponClass, 0);
}

void ASPlayerCharacter::EquipSlotTwo()
{
	if (!SecondWeaponClass) { return; }
	ChangeWeapons(SecondWeaponClass, 1);
}

void ASPlayerCharacter::EquipSlotThree()
{
	if (!ThirdWeaponClass) { return; }
	ChangeWeapons(ThirdWeaponClass, 2);
}

void ASPlayerCharacter::EquipSlotFour()
{
	if (!FourthWeaponClass) { return; }
	ChangeWeapons(FourthWeaponClass, 3);
}

void ASPlayerCharacter::EquipSlotFive()
{
	if (!FifthWeaponClass) { return; }
	ChangeWeapons(FifthWeaponClass, 4);
}