// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemPickup.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASItemPickup::ASItemPickup()
{
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetMassOverrideInKg(NAME_None, 425.f, true);
	BoxComp->SetSimulatePhysics(true);
	BoxComp->SetCanEverAffectNavigation(false);
	BoxComp->SetCollisionObjectType(ECC_WorldStatic);
	BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
	BoxComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	RootComponent = BoxComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Only hook overlap event on server
	if (GetLocalRole() == ROLE_Authority)
	{
		// Bind to overlap event on server because functionality needs to be run on server
		OnActorBeginOverlap.AddDynamic(this, &ASItemPickup::HandleBeginOverlap);
	}
}

// Only server hooks onto this event
void ASItemPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

}