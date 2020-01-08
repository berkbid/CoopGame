// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "Components/SphereComponent.h"
#include "SPlayerController.h"
#include "SUserWidgetInfoWeapon.h"
#include "SWidgetCompPickupInfo.h"
#include "Net/UnrealNetwork.h"

ASWeaponPickup::ASWeaponPickup()
{
	WeaponInfo = FWeaponInfo();
	SphereComp->SetSphereRadius(80.f, false);
}


void ASWeaponPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	APawn* OverlappedPawn = Cast<APawn>(OtherActor);
	if (!OverlappedPawn) { return; }

	HandlePickupWeapon(OverlappedPawn->GetController(), false);
}

void ASWeaponPickup::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

	HandlePickupWeapon(InteractedPC, true);
}


void ASWeaponPickup::ShowItemInfo(bool bIsVisible)
{
	Super::ShowItemInfo(bIsVisible);
	// want to take in player controller as argument and use it to determine if inventory is full!

}

// Called by server on player controller when dropping weapons
void ASWeaponPickup::SetWeaponInfo(const FWeaponInfo& NewWeaponInfo)
{
	WeaponInfo = NewWeaponInfo;

	// Update replicated variable for everyone to use on widget creation to display WeaponCurrentAmmo
	WeaponCurrentAmmo = WeaponInfo.CurrentAmmo;
}

void ASWeaponPickup::HandlePickupWeapon(AController* NewPickupController, bool bDidInteract)
{
	ASPlayerController* PC = Cast<ASPlayerController>(NewPickupController);
	if (!PC) { return; }

	// If requesting controller successfully picks up the weapon, destroy this pickup actor
	if (PC->PickedUpNewWeapon(WeaponInfo, bDidInteract))
	{
		Destroy();
	}
}


void ASWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Clients need this data which is set by server to display current ammo on widget
	DOREPLIFETIME(ASWeaponPickup, WeaponCurrentAmmo);
}