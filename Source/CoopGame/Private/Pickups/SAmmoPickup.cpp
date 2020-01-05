// Fill out your copyright notice in the Description page of Project Settings.


#include "SAmmoPickup.h"
#include "SCharacter.h"
#include "SWidgetCompPickupInfo.h"
#include "SUserWidgetInfoAmmo.h"
#include "Net/UnrealNetwork.h"

ASAmmoPickup::ASAmmoPickup()
{

	AmmoType = EAmmoType::MiniAmmo;
	AmmoAmount = 30;
}

void ASAmmoPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	RequestPickupAmmo(OtherActor);
}

void ASAmmoPickup::Interact(AActor* InteractedActor)
{
	Super::Interact(InteractedActor);
	RequestPickupAmmo(InteractedActor);
}

void ASAmmoPickup::RequestPickupAmmo(AActor* PickupActor)
{
	if (!PickupActor) { return; }
	ASCharacter* OverlappedCharacter = Cast<ASCharacter>(PickupActor);
	if (OverlappedCharacter)
	{
		int32 LeftOverAmmo = OverlappedCharacter->PickupAmmo(AmmoType, AmmoAmount);

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

void ASAmmoPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Need all players to have this value updated, and to run OnRep functionality
	DOREPLIFETIME(ASAmmoPickup, AmmoAmount);
}