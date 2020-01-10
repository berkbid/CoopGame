// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerController.h"
#include "SUserWidgetGameInfo.h"
#include "SUserWidgetInventoryInfo.h"
#include "SPlayerCharacter.h"
#include "Net/UnrealNetwork.h"
#include "SGameState.h"
#include "SPlayerState.h"
#include "Engine/World.h"
#include "SGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "SWeapon.h"
#include "CoopGame.h"
#include "DrawDebugHelpers.h"
#include "SInteractable.h"
#include "BluePrint/WidgetBlueprintLibrary.h"
#include "SWeaponPickup.h"

ASPlayerController::ASPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentSlot = 0;
	CurrentInventorySize = 0;
	InventoryMaxSize = 5;
	bIsInventoryFull = false;

	// Setup initial WeaponInventory with appropriate size and NULL values
	WeaponInventory.Init(FWeaponInfo(), InventoryMaxSize);
	
	// Setup initial ammo inventory with current and max ammo amounts
	AmmoInventory = FAmmoInfo(0, 0, 0, 0, 0, 250, 250, 250, 250, 250);

	TraceObjectQueryParams.AddObjectTypesToQuery(COLLISION_INTERACTABLEOBJECT);
}

void ASPlayerController::PostInitProperties()
{
	Super::PostInitProperties();

	// Count current inventory size
	for (int32 i = 0; i != WeaponInventory.Num(); ++i)
	{
		if (WeaponInventory[i].WeaponClass)
		{
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}
		}
	}
}

void ASPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalController())
	{
		// Update some HUD info manually here for clients since they might load in late
		if (MyGameInfo)
		{
			ASGameState* GS = GetWorld()->GetGameState<ASGameState>();
			if (GS)
			{
				MyGameInfo->SetStateText(GS->GetWaveStateString());
			}
		}
	}

}

void ASPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocalController())
	{
		// Always trace, even if dead or no pawn, this is just to display item info
		TraceForInteractables();
	}
}

void ASPlayerController::TraceForInteractables()
{
	TArray<FHitResult> HitArray;
	if (FindTraceHitArray(HitArray))
	{
		AActor* HitActor = HitArray.Last().GetActor();
		if (!HitActor) { return; }
		ASInteractable* HitInteractable = Cast<ASInteractable>(HitActor);
		if (!HitInteractable) { return; }

		// If we were previously interacting with an object, and the new object is a different object
		if (CurrentSelectedInteractable && HitInteractable != CurrentSelectedInteractable)
		{
			CurrentSelectedInteractable->HideItemInfo();
			HitInteractable->InitItemInfo(this);
			CurrentSelectedInteractable = HitInteractable;
		}
		// If we find a new interactable and were previously interacting with nothing
		else
		{
			HitInteractable->InitItemInfo(this);
			CurrentSelectedInteractable = HitInteractable;
		}
	}
	// If we didn't hit anything AND we have a current selected interactable, de-select it
	else if(CurrentSelectedInteractable)
	{
		CurrentSelectedInteractable->HideItemInfo();
		CurrentSelectedInteractable = nullptr;
	}
}

bool ASPlayerController::FindTraceHitArray(TArray<FHitResult>& OutHits)
{
	FVector EyeLocation;
	FRotator EyeRotation;
	// We override the location return in SCharacter.cpp to return camera location instead
	GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector TraceDirection = EyeRotation.Vector();
	FVector TraceStart = EyeLocation;
	FVector TraceEnd = TraceStart + (TraceDirection * 450.f);
	//DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Blue, false, .1f, 0, 5.f);

	return GetWorld()->LineTraceMultiByObjectType(OutHits, TraceStart, TraceEnd, TraceObjectQueryParams);
}

// This gets replicated when PlayerState is assigned to this controller and is valid for the first time
// GameState isn't always valid at this point
void ASPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

}

/** spawns and initializes the PlayerState for this Controller */
// Called before possess and postlogin, only server runs this code
void ASPlayerController::InitPlayerState()
{
	Super::InitPlayerState();
}

// Only PlayerState for server is valid in GameState
void ASPlayerController::ServerPostLogin()
{
	// Initialize HUD on client side
	ClientPostLogin();

	// Loop through WeaponInventory array and update HUD images if weapons are present
	int32 WeaponInventoryLen = WeaponInventory.Num();
	// Loop through our weapon inventory and populate HUD with the data
	for (int32 i = 0; i != WeaponInventoryLen; ++i)
	{
		// If there is an actual weapon in the inventory slot
		if (WeaponInventory[i].WeaponClass)
		{
			// Handle HUD picking up new weapon
			ClientPickupWeaponHUD(WeaponInventory[i], CurrentSlot, i, bIsInventoryFull);
		}
	}

	// Handle HUD changing to slot
	ClientChangeToSlotHUD(CurrentSlot);
	// Initialize ammo info for HUD
	ClientInitAmmoInventoryHUD(AmmoInventory);
}

void ASPlayerController::ClientPostLogin_Implementation()
{
	// PlayerState and GameState not valid for clients here
	// Only local controllers can add widgets
	if (IsLocalController())
	{
		// Add Game Info widget to viewport
		if (!wGameInfo) { return; }
		MyGameInfo = CreateWidget<USUserWidgetGameInfo>(this, wGameInfo);
		if (!MyGameInfo) { return; }

		// Pass reference of ourself to widget while calling setup logic on widget
		MyGameInfo->SetOwningController(this);
		MyGameInfo->AddToViewport();
	}
}

void ASPlayerController::ClientAddPlayerToHUDScoreboard_Implementation(FString const &NewPlayerName, uint32 NewPlayerNumber)
{
	if (MyGameInfo)
	{
		MyGameInfo->AddPlayerToScoreboard(NewPlayerName, NewPlayerNumber);
	}
}

void ASPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//check(InputComponent);
	// PlayerController is in charge of weapon changing, this can happen without a pawn, pawn uses the weapons
	InputComponent->BindAction("Weapon1", IE_Pressed, this, &ASPlayerController::EquipSlotOne);
	InputComponent->BindAction("Weapon2", IE_Pressed, this, &ASPlayerController::EquipSlotTwo);
	InputComponent->BindAction("Weapon3", IE_Pressed, this, &ASPlayerController::EquipSlotThree);
	InputComponent->BindAction("Weapon4", IE_Pressed, this, &ASPlayerController::EquipSlotFour);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);
	InputComponent->BindAction("Weapon5", IE_Pressed, this, &ASPlayerController::EquipSlotFive);
	InputComponent->BindAction("Interact", IE_Pressed, this, &ASPlayerController::Interact);
	InputComponent->BindAction("ToggleInventory", IE_Pressed, this, &ASPlayerController::ToggleInventory);
}

void ASPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// Uncomment to have players reset to current slot being 0 upon possessing new pawn
	//CurrentSlot = 0;
	//OnRep_SlotChange();

	// If we possess a SPlayerCharacter, equip weapon at CurrentSlot index
	ASPlayerCharacter* MySPlayerChar = Cast<ASPlayerCharacter>(GetPawn());
	if (MySPlayerChar)
	{
		// If our current slot is a valid inventory index, have player equip contents of inventory index
		int32 WeaponInventoryLen = WeaponInventory.Num();
		if (WeaponInventoryLen > CurrentSlot)
		{
			// Equip whatever weapon is in the current slot if we are a SPlayerCharacter
			MySPlayerChar->EquipWeaponClass(WeaponInventory[CurrentSlot]);
		}
	}
}

void ASPlayerController::OnUnPossess() { Super::OnUnPossess(); }

// Method that handles equipping a new weapon slot of the inventory
void ASPlayerController::EquipWeapon(uint8 NewWeaponSlot)
{
	// Only server should call this function, this is precautionary
	if (GetLocalRole() < ROLE_Authority) { return; }

	// Equip new weapon slot even if it is empty and update HUD
	if (WeaponInventory.Num() > NewWeaponSlot)
	{
		// Call ChangeWeapons, allowing for NewWeaponClass to be null going in, this will destroy current weapon
		// It is the same as changing to a new inventory slot that doesn't have a weapon in it
		ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
		if (MyPawn)
		{
			MyPawn->EquipWeaponClass(WeaponInventory[NewWeaponSlot]);
		}
		// Change active slot even if no pawn is possessed
		CurrentSlot = NewWeaponSlot;
		ClientChangeToSlotHUD(CurrentSlot);
	}
}

void ASPlayerController::Interact()
{
	// We can only interact with items if we possess a pawn
	if (!GetPawn()) { return; }
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerInteract();
		return;
	}

	TArray<FHitResult> HitArray;
	// If any hit is found
	if (FindTraceHitArray(HitArray))
	{
		AActor* HitActor = HitArray.Last().GetActor();
		if (!HitActor) { return; }
		ASInteractable* HitInteractable = Cast<ASInteractable>(HitActor);
		if (!HitInteractable) { return; }

		HitInteractable->Interact(this);
	}
}

// Try to pick up weapon if inventory has space, return success or failure
bool ASPlayerController::PickedUpNewWeapon(const FWeaponInfo& WeaponInfo, bool bDidInteract)
{
	if (GetLocalRole() < ROLE_Authority) { return false; }
	if (!bDidInteract && bIsInventoryFull) { return false; }
	ASCharacter* MyPawn = Cast<ASCharacter>(GetPawn());
	// Only pick up weapons if we have a pawn, even though we could without a pawn
	if (!MyPawn) { return false; }

	// Loop through inventory looking for empty slot
	int32 InventorySize = WeaponInventory.Num();
	for (int32 i = 0; i < InventorySize; ++i)
	{
		// If we find an empty slot, give new weapon type and update HUD image for slot
		if (!WeaponInventory[i].WeaponClass)
		{
			// Update weapon inventory slot to new weaponclass
			WeaponInventory[i] = WeaponInfo;

			// Update inventory size variable and set bIsInventoryFull
			CurrentInventorySize++;
			if (CurrentInventorySize >= InventoryMaxSize)
			{
				bIsInventoryFull = true;
			}

			// Update HUD elements for new weapon, also pass extra ammo if this weapon's ammo type
			ClientPickupWeaponHUD(WeaponInfo, CurrentSlot, i, bIsInventoryFull);

			// If we pick up a weapon into our current selected EMPTY slot, equip the weapon!
			if (CurrentSlot == i)
			{

				MyPawn->EquipWeaponClass(WeaponInfo);
				
			}
			else
			{
				// Only want to play pickup sound in this scenario, or don't want to play weapon swap sound in "if" clause above
			}
			//We officially equipped new weapon, return success
			return true;
		}
	}

	// If we don't have inventory space, BUT we interacted with weapon through E keybind, swap with current weapon slot
	if (bDidInteract)
	{
		MyPawn->EquipWeaponClass(WeaponInfo);

		FWeaponInfo OldWeaponInfo = WeaponInventory[CurrentSlot];

		// Update weapon inventory slot to new weaponclass
		WeaponInventory[CurrentSlot] = WeaponInfo;

		// Update HUD elements for new weapon, also pass extra ammo if this weapon's ammo type
		ClientPickupWeaponHUD(WeaponInfo, CurrentSlot, CurrentSlot, bIsInventoryFull);

		// Spawn old weapon and pass it the OldWeaponInfo
		if (OldWeaponInfo.WeaponPickupClass)
		{
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(MyPawn->GetActorLocation() + MyPawn->GetActorForwardVector() * 100.f);

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Owner = this;

			ASWeaponPickup* WP = GetWorld()->SpawnActorDeferred<ASWeaponPickup>(OldWeaponInfo.WeaponPickupClass, SpawnTransform, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
			if (WP)
			{
				// Need to set this while actor spawn is deferred so that listen server receives this update
				WP->SetWeaponInfo(OldWeaponInfo);
				UGameplayStatics::FinishSpawningActor(WP, WP->GetTransform());
			}
		}
		return true;
	}

	// Return failure because we didn't find empty slot for weapon
	return false;
}

int32 ASPlayerController::PickedUpNewAmmo(EAmmoType AmmoType, int32 AmmoTotal)
{
	int32 ExcessAmmo = 0;
	int32 AmmoTypeTotal = 0;

	// Try to add Ammo Total to ammo inventory, retrieve any excess amount that couldn't fit and new total of ammo type in inventory
	AmmoInventory.AddAmmo(AmmoType, AmmoTotal, ExcessAmmo, AmmoTypeTotal);

	// If we picked up ammo, thus excess ammo is less than ammo total, then we update HUD
	if (AmmoTotal > ExcessAmmo)
	{
		ClientPickupAmmoHUD(AmmoType, AmmoTypeTotal);
	}
	
	// Return to ammo pickup item how much ammo we have left over, if we picked up all AmmoTotal, the pickup will destroy itself locally
	return ExcessAmmo;
}

int32 ASPlayerController::ReloadAmmoClip()
{
	// Reference to current weapon information
	const FWeaponInfo& CurrentWeaponInfo = WeaponInventory[CurrentSlot];

	// Retrieve current clip size of current weapon
	int32 CurrentWeaponClipSize = CurrentWeaponInfo.CurrentAmmo;
	
	// Calculate ammo needed based on magazine size and current clip size
	int32 AmmmoNeeded = CurrentWeaponInfo.WeaponStats.MagazineSize - CurrentWeaponClipSize;
	if (AmmmoNeeded <= 0) { return 0; }

	// Retrieve ammo from ammo inventory
	int32 AmmoReturnAmount = 0;
	int32 ExtraAmmoTemp = 0;
	EAmmoType AmmoTypeNeeded = CurrentWeaponInfo.AmmoType;
	AmmoInventory.RequestAmmo(AmmoTypeNeeded, AmmmoNeeded, AmmoReturnAmount, ExtraAmmoTemp);

	// Update our current ammo internally
	WeaponInventory[CurrentSlot].CurrentAmmo = CurrentWeaponClipSize + AmmoReturnAmount;

	// Update HUD with new ammo inventory change
	ClientHandleReloadHUD(AmmoTypeNeeded, CurrentWeaponClipSize + AmmoReturnAmount, ExtraAmmoTemp);

	return AmmoReturnAmount;
}

void ASPlayerController::UpdateCurrentClip(int32 NewClipSize)
{
	// Update current ammo of current slot and update HUD
	WeaponInventory[CurrentSlot].CurrentAmmo = NewClipSize;
	ClientUpdateClipHUD(NewClipSize);
}

bool ASPlayerController::GetIsInventoryFull() const
{
	return bIsInventoryFull;
}

void ASPlayerController::ToggleInventory()
{
	// If we don't have MyInventoryInfo object, try to create and display it
	if (!MyInventoryInfo)
	{
		if (!wInventoryInfo) { return; }
		MyInventoryInfo = CreateWidget<USUserWidgetInventoryInfo>(this, wInventoryInfo);
		if (!MyInventoryInfo) { return; }

		if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::Hidden); }


		MyInventoryInfo->AddToViewport();
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyInventoryInfo);
		bShowMouseCursor = true;
	}
	// If we already have a MyInventoryInfo object, determine if it is in viewport and toggle off or on
	else
	{
		if (MyInventoryInfo->IsInViewport())
		{
			if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::HitTestInvisible); }

			MyInventoryInfo->RemoveFromViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
			bShowMouseCursor = false;
		}
		else
		{
			if (MyGameInfo) { MyGameInfo->SetVisibility(ESlateVisibility::Hidden); }
			MyInventoryInfo->AddToViewport();
			UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(this, MyInventoryInfo);
			bShowMouseCursor = true;
		}
	}
}

void ASPlayerController::SetStateTextHUD(FString NewState)
{
	if (MyGameInfo)
	{
		MyGameInfo->SetStateText(NewState);
	}
}

void ASPlayerController::UpdatePlayerScoreHUD(uint32 PlayerNumber, float NewScore)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerScore(PlayerNumber, NewScore);
	}
}

void ASPlayerController::UpdatePlayerKillsHUD(uint32 PlayerNumber, uint32 NewKills)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerKills(PlayerNumber, NewKills);
	}
}

void ASPlayerController::UpdatePlayerDeathsHUD(uint32 PlayerNumber, uint32 NewDeaths)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdatePlayerDeaths(PlayerNumber, NewDeaths);
	}
}

void ASPlayerController::EquipSlotOne()
{
	ServerEquipWeaponSlot(0);
}

void ASPlayerController::EquipSlotTwo()
{
	ServerEquipWeaponSlot(1);
}

void ASPlayerController::EquipSlotThree()
{
	ServerEquipWeaponSlot(2);
}

void ASPlayerController::EquipSlotFour()
{
	ServerEquipWeaponSlot(3);
}

void ASPlayerController::EquipSlotFive()
{
	ServerEquipWeaponSlot(4);
}

void ASPlayerController::ServerEquipWeaponSlot_Implementation(uint8 SlotToEquip)
{
	if (CurrentSlot == SlotToEquip) { return; }

	EquipWeapon(SlotToEquip);
}

bool ASPlayerController::ServerEquipWeaponSlot_Validate(uint8 SlotToEquip)
{
	if (SlotToEquip < 0 || SlotToEquip > InventoryMaxSize)
	{
		return false;
	}
	return true;
}

void ASPlayerController::ServerInteract_Implementation()
{
	Interact();
}

void ASPlayerController::ClientInitAmmoInventoryHUD_Implementation(const FAmmoInfo& NewAmmoInfo)
{
	if (MyGameInfo)
	{
		MyGameInfo->InitAmmoInventory(NewAmmoInfo);
	}
}

void ASPlayerController::ClientChangeToSlotHUD_Implementation(int32 NewSlot)
{
	if (!MyGameInfo) { return; }

	// Call HUD method to change slot with ammo info
	MyGameInfo->InventoryChangeToSlot(NewSlot);
}

void ASPlayerController::ClientHandleReloadHUD_Implementation(EAmmoType NewAmmoType, int32 NewClipAmmo, int32 NewExtraAmmo)
{
	if (!MyGameInfo) { return; }

	MyGameInfo->HandleReloadAmmoType(NewAmmoType, NewClipAmmo, NewExtraAmmo);

}

// This update for HUD is equivalent to equipping and un-equipping the weapon, show weapon image or remove it
void ASPlayerController::ClientPickupWeaponHUD_Implementation(const FWeaponInfo& WeaponInfo, int32 TempCurrentSlot, int32 SlotToUpdate, bool bAreWeFull)
{
	// Update this boolean ourself as we get it passed from server
	bIsInventoryFull = bAreWeFull;

	// Handle HUD for picking up new weapon
	if (!MyGameInfo) { return; }

	// THis uses weaponinfo for current ammo which is wrong
	// Set initial HUD state for weapon slot, including picture and ammo amount
	MyGameInfo->HandlePickupWeapon(SlotToUpdate, WeaponInfo);

	// Only play weapon pickup sound if we aren't already selecting on new weapon slot, this is because weapon swap sound will play
	if (TempCurrentSlot != SlotToUpdate)
	{
		// Play pickup sound also
		if (PickupWeaponSound)
		{
			UGameplayStatics::PlaySound2D(this, PickupWeaponSound);
			
		}
	}
}

void ASPlayerController::ClientPickupAmmoHUD_Implementation(EAmmoType NewAmmoType, int32 NewExtraAmmo)
{
	if (MyGameInfo)
	{
		MyGameInfo->HandlePickupAmmo(NewAmmoType, NewExtraAmmo);
	}

	// Play pickup sound also
	if (PickupAmmoSound)
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PickupAmmoSound, ControlledPawn->GetActorLocation());
		}
	}
}

void ASPlayerController::ClientUpdateClipHUD_Implementation(int32 CurrentAmmo)
{
	if (MyGameInfo)
	{
		MyGameInfo->UpdateCurrentClipAmmo(CurrentAmmo);
	}
}

void ASPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}