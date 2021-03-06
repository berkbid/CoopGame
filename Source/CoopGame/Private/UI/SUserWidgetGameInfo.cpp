// Fill out your copyright notice in the Description page of Project Settings.


#include "SUserWidgetGameInfo.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameState.h"
#include "GameFramework/PlayerState.h"
#include "Engine/Texture2D.h"
#include "SWeapon.h"
#include "WidgetInventoryHUD.h"
#include "Scoreboard/WidgetScoreboardHUD.h"
#include "WidgetInventoryPage.h"

void USUserWidgetGameInfo::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	// Send inventory slot reference array to inventory info page to store duplicate
	if (InventoryInfoPage && InventoryHUD)
	{
		InventoryInfoPage->GetInventorySlotReferences(InventoryHUD->GetInventorySlotWidgets());
	}
}

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

bool USUserWidgetGameInfo::ToggleInventoryPage()
{
	if (InventoryInfoPage && InventoryHUD)
	{
		if (InventoryInfoPage->IsVisible())
		{
			InventoryHUD->InitInventoryHUD();

			InventoryInfoPage->SetVisibility(ESlateVisibility::Hidden);
			InventoryHUD->SetVisibility(ESlateVisibility::HitTestInvisible);
			return false;
		}
		else
		{
			InventoryInfoPage->InitInventoryPage();

			// Must set visibility of InfoPage to "Visible" so that it can be interacted with mouse since it contains a Canvas Panel
			InventoryInfoPage->SetVisibility(ESlateVisibility::Visible);
			InventoryHUD->SetVisibility(ESlateVisibility::Hidden);
			return true;
		}
	}

	return false;
}

// When player picks up a weapon, update inventory with weapon picture and Slot Ammo amount
void USUserWidgetGameInfo::HandlePickupWeapon(int32 WeaponSlot, const FWeaponInfo& NewWeaponInfo)
{
	if (InventoryHUD)
	{
		// Want to see if we picked up weapon that was in our currently active slot
		InventoryHUD->HandlePickupWeapon(WeaponSlot, NewWeaponInfo, WeaponToTextureMap, AmmoToTextureMap);
	}
}

void USUserWidgetGameInfo::HandlePickupAmmo(EAmmoType NewAmmoType, int32 ExtraAmmo)
{
	if (InventoryHUD)
	{
		InventoryHUD->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}
}

// When player reloads, update all ammo text and other weapon slots sharing same ammo type needs updating
void USUserWidgetGameInfo::HandleReloadAmmoType(EAmmoType NewAmmoType, int32 CurrentAmmo, int32 ExtraAmmo)
{
	if (InventoryHUD)
	{
		// Update current slot ammo now takes care of updating current ammo for weapon info call above
		InventoryHUD->UpdateCurrentSlotAmmo(CurrentAmmo);
		InventoryHUD->UpdateAmmoTypeAmount(NewAmmoType, ExtraAmmo);
	}
}

// When player shoots and current clip ammo changes, send data to relevant HUD objects
void USUserWidgetGameInfo::UpdateCurrentClipAmmo(int32 NewCurrentAmmo)
{
	if (InventoryHUD)
	{
		InventoryHUD->UpdateCurrentSlotAmmo(NewCurrentAmmo);
	}
}

// When player changes inventory slots, handle container visual and current weapon info
void USUserWidgetGameInfo::InventoryChangeToSlot(int32 WeaponSlot)
{
	if (InventoryHUD)
	{
		InventoryHUD->HandleSlotChange(WeaponSlot);
	}
}

void USUserWidgetGameInfo::InitAmmoInventory(const FAmmoInfo& StartingAmmoInfo)
{
	if (InventoryHUD)
	{
		InventoryHUD->InitAmmoInventory(StartingAmmoInfo);
	}
}

void USUserWidgetGameInfo::AddPlayerToScoreboard(FString NewPlayerName, uint32 NewPlayerNumber)
{
	if (ScoreboardHUD)
	{
		ScoreboardHUD->AddPlayerToScoreboard(NewPlayerName, NewPlayerNumber);
	}
}

void USUserWidgetGameInfo::UpdatePlayerScore(uint32 PlayerNumber, float NewScore)
{
	if (ScoreboardHUD)
	{
		ScoreboardHUD->UpdatePlayerScore(PlayerNumber, NewScore);
	}
}

void USUserWidgetGameInfo::UpdatePlayerKills(uint32 PlayerNumber, uint32 NewKills)
{
	if (ScoreboardHUD)
	{
		ScoreboardHUD->UpdatePlayerKills(PlayerNumber, NewKills);
	}
}

void USUserWidgetGameInfo::UpdatePlayerDeaths(uint32 PlayerNumber, uint32 NewDeaths)
{
	if (ScoreboardHUD)
	{
		ScoreboardHUD->UpdatePlayerDeaths(PlayerNumber, NewDeaths);
	}
}

void USUserWidgetGameInfo::SetStateText(FString NewState)
{
	if (StateText)
	{
		StateText->SetText(FText::FromString(NewState));
	}
}