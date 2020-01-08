// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractable.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SWidgetCompPickupInfo.h"
#include "CoopGame.h"

// Sets default values
ASInteractable::ASInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	// Box component for collision with surfaces
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetMassOverrideInKg(NAME_None, 425.f, true);
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetGenerateOverlapEvents(false);
	BoxComp->SetCanEverAffectNavigation(false);
	BoxComp->SetCollisionObjectType(ECC_WorldStatic);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = BoxComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetupAttachment(RootComponent);

	// Sphere component for line traces from players
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionObjectType(COLLISION_INTERACTABLEOBJECT);
	SphereComp->SetGenerateOverlapEvents(false);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetupAttachment(RootComponent);

	// Widget component to display info of interactable item to player upon line trace with sphere component
	InfoWidget = CreateDefaultSubobject<USWidgetCompPickupInfo>(TEXT("InfoWidget"));
	InfoWidget->SetupAttachment(RootComponent);
	InfoWidget->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	SetReplicates(true);

	NetUpdateFrequency = 60.f;
	MinNetUpdateFrequency = 20.f;
	NetPriority = .7f;
	NetCullDistanceSquared = 65000000.f;
}

void ASInteractable::ShowItemInfo(bool bIsVisible)
{
	if (bIsVisible)
	{
		SetInfoVisible();
	}
	else
	{
		SetInfoInvisible();
	}
}

void ASInteractable::Interact(APlayerController* InteractedPC)
{
	if (!InteractedPC) { return; }
}

void ASInteractable::SetInfoVisible()
{
	if (bIsInfoVisible) { return; }

	if (InfoWidget)
	{
		InfoWidget->SetInfoVisible(true);
		bIsInfoVisible = true;
	}
}

void ASInteractable::SetInfoInvisible()
{
	if (!bIsInfoVisible) { return; }

	if (InfoWidget)
	{
		InfoWidget->SetInfoVisible(false);
		bIsInfoVisible = false;
	}
}