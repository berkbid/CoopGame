// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"
#include "SUserWidgetPlayerStats.h"
#include "SOverlayInventorySlot.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Texture2D.h"
#include "SWeapon.h"
#include "SHorizontalBoxInventory.h"
#include "SVerticalBoxCurrentWeapon.h"

bool USUserWidgetGameInfo::Initialize()
{
	// Bind to delegates of widget components here
	// eg OkayButton->OnClicked.AddDynamic(this, &UMainMenu::BeginGame);

	// For some reason Super::Initialize() call doesn't work
	return UUserWidget::Initialize();

}

// Player controller calls this after creating this widget and passes reference to itself
void USUserWidgetGameInfo::SetOwningController(APlayerController* NewController)
{
	// Skip repeated calls
	if (OwningController == NewController) { return; }
	OwningController = NewController;
}

// When player picks up a weapon, update inventory with weapon picture and Slot Ammo amount
void USUserWidgetGameInfo::HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo)
{
	// Find texture associated with weapon class we picked up
	UTexture2D** TempWeaponTexture = WeaponToTextureMap.Find(NewWeaponInfo.WeaponType);

	if (InventoryContainer && TempWeaponTexture)
	{
		// Want to see if we picked up weapon that was in our currently active slot
		bool bPickupInCurrentSlot = false;

		// This is passing incorrect current ammo inside of NewWeaponInfo
		bPickupInCurrentSlot = InventoryContainer->HandlePickupWeapon(WeaponSlot, NewWeaponInfo, *TempWeaponTexture);

		// Need to setup WeaponInfo if we picked up a weapon in our current slot
		if (bPickupInCurrentSlot)
		{
			// Query for extra ammo for current slot and update weapon info
			int32 NewExtraAmmo = InventoryContainer->GetExtraAmmoOfType(NewWeaponInfo.AmmoType);
			UpdateWeaponInfo(NewWeaponInfo, NewExtraAmmo);
		}
	}
}

// shouldnt have to be given extra ammo amount
void USUserWidgetGameInfo::HandlePickupAmmo(EAmmoType NewAmmoType, int32 ExtraAmmo)
{
	// Tell inventory new current ammo and extra ammo amount (current slot is assumed by inventory)
	if (InventoryContainer)
	{
		// Update slot ammo text for all slots that share same ammo type since reload altered this amount
		InventoryContainer->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}

	// If our current weapon shares the ammo type, then update its ammo
	if (CurrentWeaponInfo)
	{
		CurrentWeaponInfo->QueryToSetExtraAmmo(NewAmmoType, ExtraAmmo);
	}

	// Update extra ammo text
	switch (NewAmmoType)
	{
	case EAmmoType::MiniAmmo:
		SetMiniAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::MediumAmmo:
		SetMediumAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::HeavyAmmo:
		SetHeavyAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::ShellAmmo:
		SetShellAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::RocketAmmo:
		SetRocketAmmoText(FString::FromInt(ExtraAmmo));
		break;
	default:
		break;
	}
}

// When player reloads, update all ammo text and other weapon slots sharing same ammo type needs updating
void USUserWidgetGameInfo::HandleReloadAmmoType(EAmmoType NewAmmoType, int32 CurrentAmmo, int32 ExtraAmmo)
{
	// Update current slot ammo and ammo type ammo for all slots who share that ammo type
	if (InventoryContainer)
	{
		InventoryContainer->UpdateCurrentSlotAmmo(CurrentAmmo);
		InventoryContainer->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}

	// Update the weapon info HUD with new ammo values
	if (CurrentWeaponInfo)
	{
		CurrentWeaponInfo->SetBothAmmo(CurrentAmmo, ExtraAmmo);
	}

	// Update extra ammo text
	switch (NewAmmoType)
	{
	case EAmmoType::MiniAmmo:
		SetMiniAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::MediumAmmo:
		SetMediumAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::HeavyAmmo:
		SetHeavyAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::ShellAmmo:
		SetShellAmmoText(FString::FromInt(ExtraAmmo));
		break;
	case EAmmoType::RocketAmmo:
		SetRocketAmmoText(FString::FromInt(ExtraAmmo));
		break;
	default:
		break;
	}
}

// When player shoots and current clip ammo changes, send data to relevant HUD objects
void USUserWidgetGameInfo::UpdateCurrentClipAmmo(int32 NewCurrentAmmo)
{
	if (InventoryContainer)
	{
		InventoryContainer->UpdateCurrentSlotAmmo(NewCurrentAmmo);
	}

	if (CurrentWeaponInfo)
	{
		CurrentWeaponInfo->SetWeaponCurrentAmmo(NewCurrentAmmo);
	}
}

// When player changes inventory slots, handle container visual and current weapon info
void USUserWidgetGameInfo::InventoryChangeToSlot(int32 WeaponSlot)
{
	// Handle inventory visual for changing slot 
	if (InventoryContainer)
	{
		// Grab values from slot to use for CurrentWeaponInfo updating
		FWeaponInfo NewWeaponInfo;
		int32 SlotExtraAmmo;

		// Change to slot and get how much extra ammo is in slot in order to update weapon info
		InventoryContainer->HandleSlotChange(WeaponSlot, NewWeaponInfo, SlotExtraAmmo);

		// Update WeaponInfo
		UpdateWeaponInfo(NewWeaponInfo, SlotExtraAmmo);
	}
}

// This is a special case where we picked up a weapon in an already selected slot, we just need to update weapon info
void USUserWidgetGameInfo::UpdateWeaponInfo(const FWeaponInfo& NewWeaponInfo, int32 NewExtraAmmo)
{
	// be given the slot number, and get weapon info and extra ammo from the inventory by query, ALREADY DO
	if (CurrentWeaponInfo)
	{
		// Setup current weapon info from weaponinfo
		CurrentWeaponInfo->InitWeaponInfo(NewWeaponInfo, NewExtraAmmo);
	}
}

void USUserWidgetGameInfo::AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber)
{
	if (wPlayerStats)
	{
		USUserWidgetPlayerStats* NewPlayerStats = CreateWidget<USUserWidgetPlayerStats>(this, wPlayerStats);
		if (NewPlayerStats && ScoreboardEntryBox)
		{
			NewPlayerStats->SetAllText(NewPlayerName, FString("0"), FString("0"), FString("0"));
			ScoreboardEntryBox->AddChild(NewPlayerStats);

			// Add reference to PlayerStats to ScoreboardDictionary for updating in the future
			ScoreboardDictionary.Add(NewPlayerNumber, NewPlayerStats);
		}
	}
}

void USUserWidgetGameInfo::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetScoreText(FString::SanitizeFloat(NewScore));
	}
}

void USUserWidgetGameInfo::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetKillText(FString::FromInt(NewKills));
	}
}

void USUserWidgetGameInfo::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	USUserWidgetPlayerStats** PlayerStats = ScoreboardDictionary.Find(PlayerNumber);

	if (PlayerStats)
	{
		(*PlayerStats)->SetDeathText(FString::FromInt(NewDeaths));
	}
}

void USUserWidgetGameInfo::SetStateText(FString NewState)
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(NewState));
	}
}

void USUserWidgetGameInfo::SetMiniAmmoText(FString NewText)
{
	if (MiniAmmoText)
	{

		MiniAmmoText->SetText(FText::FromString("Mini: " + NewText));
	}
}

void USUserWidgetGameInfo::SetMediumAmmoText(FString NewText)
{
	if (MediumAmmoText)
	{
		MediumAmmoText->SetText(FText::FromString("Medium: " + NewText));
	}
}

void USUserWidgetGameInfo::SetHeavyAmmoText(FString NewText)
{
	if (HeavyAmmoText)
	{
		HeavyAmmoText->SetText(FText::FromString("Heavy: " + NewText));
	}
}

void USUserWidgetGameInfo::SetShellAmmoText(FString NewText)
{
	if (ShellAmmoText)
	{
		ShellAmmoText->SetText(FText::FromString("Shell: " + NewText));
	}
}

void USUserWidgetGameInfo::SetRocketAmmoText(FString NewText)
{
	if (RocketAmmoText)
	{
		RocketAmmoText->SetText(FText::FromString("Rocket: " + NewText));
	}
}