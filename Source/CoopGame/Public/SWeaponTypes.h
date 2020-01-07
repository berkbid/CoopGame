#pragma once

#include "SweaponTypes.generated.h"


UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	MiniAmmo,
	MediumAmmo,
	HeavyAmmo,
	ShellAmmo,
	RocketAmmo
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	AssaultRifle,
	GrenadeLauncher
};

UENUM(BlueprintType)
enum class EWeaponRarity : uint8
{
	Common,
	Uncommon,
	Rare,
	Epic,
	Legendary
};

USTRUCT(BlueprintType)
struct FAmmoInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MiniCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MediumCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 HeavyCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 ShellCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 RocketCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxMiniAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxMediumAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxHeavyAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxShellAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ammo")
	int32 MaxRocketAmmo;


	/* Helper function for retrieving ammo of a certain type, passes back how much ammo could be returned and new total for ammo type*/
	void RequestAmmo(EAmmoType AmmoTypeNeeded, int32 AmmoAmountRequested, int32& AmmoReturned, int32& NewAmmoTotal)
	{
		int32 ExtraAmmoTemp = 0;
		int32 AmmoReturnAmount = 0;

		switch (AmmoTypeNeeded)
		{
		case EAmmoType::MiniAmmo:
			ExtraAmmoTemp = MiniCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				MiniCount -= AmmoReturnAmount;
				ExtraAmmoTemp = MiniCount;
			}
			break;
		case EAmmoType::MediumAmmo:
			ExtraAmmoTemp = MediumCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				MediumCount -= AmmoReturnAmount;
				ExtraAmmoTemp = MediumCount;
			}
			break;
		case EAmmoType::HeavyAmmo:
			ExtraAmmoTemp = HeavyCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				HeavyCount -= AmmoReturnAmount;
				ExtraAmmoTemp = HeavyCount;
			}
			break;
		case EAmmoType::ShellAmmo:
			ExtraAmmoTemp = ShellCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				ShellCount -= AmmoReturnAmount;
				ExtraAmmoTemp = ShellCount;
			}
			break;
		case EAmmoType::RocketAmmo:
			ExtraAmmoTemp = RocketCount;
			if (ExtraAmmoTemp > 0)
			{
				AmmoReturnAmount = FMath::Min(AmmoAmountRequested, ExtraAmmoTemp);
				RocketCount -= AmmoReturnAmount;
				ExtraAmmoTemp = RocketCount;
			}
			break;
		default:
			break;
		}
		AmmoReturned = AmmoReturnAmount;
		NewAmmoTotal = ExtraAmmoTemp;
	}

	/* Function to add ammo, returns left over ammo if full of specified type, and new ammo total of specified type */
	void AddAmmo(EAmmoType NewAmmoType, int32 AmmoAmount, int32& LeftOverAmmo, int32& NewAmmoTotal)
	{
		switch (NewAmmoType)
		{
		case EAmmoType::MiniAmmo:
			if (MiniCount + AmmoAmount > MaxMiniAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxMiniAmmo - MiniCount);
				MiniCount = MaxMiniAmmo;
				NewAmmoTotal = MiniCount;
			}
			else
			{
				LeftOverAmmo = 0;
				MiniCount += AmmoAmount;
				NewAmmoTotal = MiniCount;
			}
			break;
		case EAmmoType::MediumAmmo:
			if (MediumCount + AmmoAmount > MaxMediumAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxMediumAmmo - MediumCount);
				MediumCount = MaxMediumAmmo;
				NewAmmoTotal = MediumCount;
			}
			else
			{
				LeftOverAmmo = 0;
				MediumCount += AmmoAmount;
				NewAmmoTotal = MediumCount;
			}
			break;
		case EAmmoType::HeavyAmmo:
			if (HeavyCount + AmmoAmount > MaxHeavyAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxHeavyAmmo - HeavyCount);
				HeavyCount = MaxHeavyAmmo;
				NewAmmoTotal = HeavyCount;
			}
			else
			{
				LeftOverAmmo = 0;
				HeavyCount += AmmoAmount;
				NewAmmoTotal = HeavyCount;
			}
			break;
		case EAmmoType::ShellAmmo:
			if (ShellCount + AmmoAmount > MaxShellAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxShellAmmo - ShellCount);
				ShellCount = MaxShellAmmo;
				NewAmmoTotal = ShellCount;
			}
			else
			{
				LeftOverAmmo = 0;
				ShellCount += AmmoAmount;
				NewAmmoTotal = ShellCount;
			}
			break;
		case EAmmoType::RocketAmmo:
			if (RocketCount + AmmoAmount > MaxRocketAmmo)
			{
				LeftOverAmmo = AmmoAmount - (MaxRocketAmmo - RocketCount);
				RocketCount = MaxRocketAmmo;
				NewAmmoTotal = RocketCount;
			}
			else
			{
				LeftOverAmmo = 0;
				RocketCount += AmmoAmount;
				NewAmmoTotal = RocketCount;
			}
			break;
		default:
			break;
		}
	}

	void SetAmmoTypeAmount(EAmmoType NewAmmoType, int32 NewAmmoAmount)
	{
		switch (NewAmmoType)
		{
		case EAmmoType::MiniAmmo:
			MiniCount = NewAmmoAmount;
			break;
		case EAmmoType::MediumAmmo:
			MediumCount = NewAmmoAmount;
			break;
		case EAmmoType::HeavyAmmo:
			HeavyCount = NewAmmoAmount;
			break;
		case EAmmoType::ShellAmmo:
			ShellCount = NewAmmoAmount;
			break;
		case EAmmoType::RocketAmmo:
			RocketCount = NewAmmoAmount;
			break;
		default:
			break;
		}
	}

	int32 GetAmmoTypeAmount(EAmmoType NewAmmoType)
	{
		switch (NewAmmoType)
		{
		case EAmmoType::MiniAmmo:
			return MiniCount;
			break;
		case EAmmoType::MediumAmmo:
			return MediumCount;
			break;
		case EAmmoType::HeavyAmmo:
			return HeavyCount;
			break;
		case EAmmoType::ShellAmmo:
			return ShellCount;
			break;
		case EAmmoType::RocketAmmo:
			return RocketCount;
			break;
		default:
			break;
		}
		return 0;
	}

	// For Garbage Cleanup
	void Destroy()
	{
	}

	// Default constructor
	FAmmoInfo()
	{
		MiniCount = 0;
		MediumCount = 0;
		HeavyCount = 0;
		ShellCount = 0;
		RocketCount = 0;

		MaxMiniAmmo = 500;
		MaxMediumAmmo = 500;
		MaxHeavyAmmo = 500;
		MaxShellAmmo = 500;
		MaxRocketAmmo = 500;
	}

	FAmmoInfo(int32 NewMiniAmmo, int32 NewMediumAmmo, int32 NewHeavyAmmo, int32 NewShellAmmo, int32 NewRocketAmmo,
		int32 NewMaxMiniAmmo, int32 NewMaxMediumAmmo, int32 NewMaxHeavyAmmo, int32 NewMaxShellAmmo, int32 NewMaxRocketAmmo)
	{
		MiniCount = NewMiniAmmo;
		MediumCount = NewMediumAmmo;
		HeavyCount = NewHeavyAmmo;
		ShellCount = NewShellAmmo;
		RocketCount = NewRocketAmmo;

		MaxMiniAmmo = NewMaxMiniAmmo;
		MaxMediumAmmo = NewMaxMediumAmmo;
		MaxHeavyAmmo = NewMaxHeavyAmmo;
		MaxShellAmmo = NewMaxShellAmmo;
		MaxRocketAmmo = NewMaxRocketAmmo;
	}
};

USTRUCT(BlueprintType)
struct FWeaponStats
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MagazineSize;

	FWeaponStats()
	{
		BaseDamage = 20.f;
		FireRate = 0.3f;
		ReloadTime = 2.f;
		MagazineSize = 30;
	}

	FWeaponStats(float NewBaseDamage, float NewFireRate, float NewReloadTime, int32 NewMagazineSize)
	{
		BaseDamage = NewBaseDamage;
		FireRate = NewFireRate;
		ReloadTime = NewReloadTime;
		MagazineSize = NewMagazineSize;
	}
};

USTRUCT(BlueprintType)
struct FWeaponInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UClass* WeaponType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EWeaponRarity WeaponRarity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FWeaponStats WeaponStats;

	// For Garbage Cleanup
	void Destroy()
	{
		WeaponType = NULL;
	}

	// Default constructor
	FWeaponInfo()
	{
		WeaponType = NULL;
		WeaponName = FName();
		WeaponRarity = EWeaponRarity::Common;
		AmmoType = EAmmoType::MiniAmmo;
		CurrentAmmo = 0;
		WeaponStats = FWeaponStats();
	}

	// Constructor with parameters for properties
	FWeaponInfo(UClass* NewWeaponClass, FName NewWeaponName, EWeaponRarity NewWeaponRarity, EAmmoType NewAmmoType, int32 NewCurrentAmmo, const FWeaponStats &NewWeaponStats)
	{
		WeaponType = NewWeaponClass;
		WeaponName = NewWeaponName;
		WeaponRarity = NewWeaponRarity;
		AmmoType = NewAmmoType;
		CurrentAmmo = NewCurrentAmmo;
		WeaponStats = NewWeaponStats;
	}
};

USTRUCT(BlueprintType)
struct FWeaponPickup
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	uint8 DropChance;

	FWeaponPickup()
	{
		WeaponType = EWeaponType::AssaultRifle;
		DropChance = 1;
	}

	FWeaponPickup(EWeaponType NewWeaponType, uint8 NewDropChance)
	{
		WeaponType = NewWeaponType;
		DropChance = NewDropChance;
	}
};