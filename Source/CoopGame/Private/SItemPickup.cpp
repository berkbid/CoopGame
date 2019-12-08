// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemPickup.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"
#include "SWeapon.h"

// Sets default values
ASItemPickup::ASItemPickup()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->SetCanEverAffectNavigation(false);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CapsuleComp;

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CapsuleComp);


	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Only hook Explode timer on server
	if (Role == ROLE_Authority)
	{
		// Bind to overlap event on server because functionality needs to be run on server
		OnActorBeginOverlap.AddDynamic(this, &ASItemPickup::HandleBeginOverlap);

	}
	
}

void ASItemPickup::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("Overlapped Actor"));

	if (ItemType)
	{
		ASCharacter* OverlappedCharacter = Cast<ASCharacter>(OtherActor);
		if (OverlappedCharacter)
		{
			OverlappedCharacter->PickupWeapon(ItemType);
		}
	}
}

