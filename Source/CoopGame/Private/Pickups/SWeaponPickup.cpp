// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeaponPickup.h"
#include "SCharacter.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"

ASWeaponPickup::ASWeaponPickup()
{
	WeaponInfo = FWeaponInfo();
	MaterialIndexToChange = 0;
}

void ASWeaponPickup::PostInitProperties()
{
	Super::PostInitProperties();
}

void ASWeaponPickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (MatInst) { MatInst = nullptr; }

	switch (WeaponInfo.WeaponRarity)
	{
	case EWeaponRarity::Common:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(.18f, .18f, .18f, 1.f));
		}

		break;
	case EWeaponRarity::Uncommon:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			// This variable "LastTimeDamageTaken" is set in the editor in the graph for the material
			MatInst->SetVectorParameterValue("Color", FLinearColor(0.f, .1f, 0.f, 1.f));
		}
		break;
	case EWeaponRarity::Rare:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));

		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(0.f, 0.f, 1.f, 1.f));
		}
		break;
	case EWeaponRarity::Epic:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(1.f, 0.f, 1.f, 1.f));
		}
		break;
	case EWeaponRarity::Legendary:
		// Need to create dynamic instance so changes on only the one instance of the material, not all instances
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(MaterialIndexToChange, MeshComp->GetMaterial(MaterialIndexToChange));
		if (MatInst)
		{
			MatInst->SetVectorParameterValue("Color", FLinearColor(1.f, 1.f, 0.f, 1.f));
		}
		break;
	default:
		break;
	}
}

void ASWeaponPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (WeaponInfo.WeaponType)
	{
		ASCharacter* OverlappedCharacter = Cast<ASCharacter>(OtherActor);
		if (OverlappedCharacter)
		{
			OverlappedCharacter->PickupWeapon(WeaponInfo, this);
		}
	}
}
