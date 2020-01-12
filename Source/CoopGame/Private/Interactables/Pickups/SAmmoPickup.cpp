// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoPickup.h"
#include "SWidgetCompPickupInfo.h"
#include "SUserWidgetInfoAmmo.h"
#include "Net/UnrealNetwork.h"
#include "SPlayerController.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

ASAmmoPickup::ASAmmoPickup()
{
	BoxComp->SetBoxExtent(FVector(15.f, 8.f, 8.f), false);
	SphereComp->SetSphereRadius(40.f, false);

	AmmoType = EAmmoType::MiniAmmo;
	AmmoAmount = 30;
}

void ASAmmoPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor) { return; }

	APawn* OverlappedPawn = Cast<APawn>(OtherActor);
	if (OverlappedPawn)
	{
		HandleAmmoPickup(OverlappedPawn->GetController());
	}
}

void ASAmmoPickup::Interact(APlayerController* InteractedPC)
{
	Super::Interact(InteractedPC);

	HandleAmmoPickup(InteractedPC);
}

void ASAmmoPickup::HandleAmmoPickup(AController* NewPickupController)
{
	ASPlayerController* PC = Cast<ASPlayerController>(NewPickupController);

	if (!PC) { return; }

	int32 LeftOverAmmo = PC->PickedUpNewAmmo(AmmoType, AmmoAmount);
	// If player didn't pick up any ammo, don't need to do anything
	if (LeftOverAmmo == AmmoAmount) { return; }

	if (LeftOverAmmo <= 0)
	{
		Destroy();
	}
	else
	{
		AmmoAmount = LeftOverAmmo;
		OnRep_AmmoAmount();
	}
}

// Need everyone to set new ammo text for this ammo pickups widget component
void ASAmmoPickup::OnRep_AmmoAmount()
{
	// Get InfoWidget Component, it is generalized for parent WidgetInst types, so we cast and call specific Ammo method
	if (InfoWidget)
	{
		USUserWidgetInfoAmmo* WA = Cast<USUserWidgetInfoAmmo>(InfoWidget->WidgetInfoInst);
		if (WA)
		{
			WA->SetAmmoText(AmmoAmount);
		}
	}
}

void ASAmmoPickup::InitItemInfo(const ASPlayerController* ClientController)
{
	bool bFull = ClientController->GetIsAmmoTypeFull(AmmoType);

	// This will prevent setting the widget text every time and only when value has changed by keeping temp value client side
	if (bFull != bIsInventoryFullTemp) {
		if (InfoWidget)
		{
			USUserWidgetInfoAmmo* UW = Cast<USUserWidgetInfoAmmo>(InfoWidget->WidgetInfoInst);
			if (UW)
			{
				if (bFull)
				{
					UW->SetFullState();
				}
				else
				{
					UW->SetNotFullState();
				}
				// If we successfully changed the text, then we update our local client variable
				bIsInventoryFullTemp = bFull;
			}
		}
	}

	// Always call super to handle general item info display behavior, after setting values above
	Super::InitItemInfo(ClientController);
}

void ASAmmoPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Need all players to have this value updated, and to run OnRep functionality
	DOREPLIFETIME(ASAmmoPickup, AmmoAmount);
}