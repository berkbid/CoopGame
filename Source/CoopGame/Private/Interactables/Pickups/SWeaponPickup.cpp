// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "Components/SphereComponent.h"
#include "SPlayerController.h"
#include "SUserWidgetInfoWeapon.h"
#include "SWidgetCompPickupInfo.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"

ASWeaponPickup::ASWeaponPickup()
{
	WeaponInfo = FWeaponInfo();
	WeaponCurrentAmmo = 0;
	SphereComp->SetSphereRadius(80.f, false);
}

void ASWeaponPickup::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// Only server needs to update this value
	if (GetLocalRole() == ROLE_Authority)
	{
		// If this variable was altered on per-instance basis, then update currentammo in WeaponInfo
		if (WeaponCurrentAmmo > 0)
		{
			// If WeaponCurrentAmmo was updated in the editor, update the value in the struct as well which gets passed to the player
			WeaponInfo.CurrentAmmo = WeaponCurrentAmmo;
		}
	}
}

//void ASWeaponPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
//{
//	Super::PostEditChangeProperty(PropertyChangedEvent);
//
//	if (PropertyChangedEvent.Property != nullptr)
//	{
//		// If we want to replicate movement, set RemoteRole to match
//		if (PropertyChangedEvent.Property->GetFName() == FName(TEXT("WeaponCurrentAmmo")))
//		{
//			WeaponInfo.CurrentAmmo = WeaponCurrentAmmo;
//		}
//	}
//}

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

// Called by server from player controller when dropping weapons
void ASWeaponPickup::SetWeaponInfo(const FWeaponInfo &NewWeaponInfo)
{
	// Update replicated variable for everyone to use on widget creation to display WeaponCurrentAmmo
	WeaponCurrentAmmo = NewWeaponInfo.CurrentAmmo;

	WeaponInfo = NewWeaponInfo;
}

// Server is in this function
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

void ASWeaponPickup::InitItemInfo(const ASPlayerController* ClientController)
{
	// check inventory space and change widget text if need
	bool bFull = ClientController->GetIsInventoryFull();

	// This will prevent setting the widget text every time and only when value has changed by keeping temp value client side
	if (bFull != bIsInventoryFullTemp) {
		if (InfoWidget)
		{
			USUserWidgetInfoWeapon* UW = Cast<USUserWidgetInfoWeapon>(InfoWidget->WidgetInfoInst);
			if (UW)
			{
				if (bFull)
				{
					UW->SetPickupText("Swap");
				}
				else
				{
					UW->SetPickupText("Pick Up");
				}
				// If we successfully changed the text, then we update our local client variable
				bIsInventoryFullTemp = bFull;
			}
		}
	}

	// Always call super to handle general item info display behavior, after setting values above
	Super::InitItemInfo(ClientController);
}

void ASWeaponPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Clients need this data which is set by server to display current ammo on widget
	DOREPLIFETIME(ASWeaponPickup, WeaponCurrentAmmo);

}