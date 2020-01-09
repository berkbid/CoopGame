// Fill out your copyright notice in the Description page of Project Settings.


#include "SGrenadeProjectile.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Engine/Player.h"
#include "Components/SphereComponent.h"

// Sets default values
ASGrenadeProjectile::ASGrenadeProjectile()
{

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// Set to physics body to let radial component affect us (eg. when a nearby barrel explodes)
	MeshComp->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComp->SetGenerateOverlapEvents(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetGenerateOverlapEvents(true);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);

	SphereComp->SetupAttachment(RootComponent);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 2000.f;
	MovementComp->MaxSpeed = 2000.f;
	MovementComp->bShouldBounce = true;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(Cast<USceneComponent>(MeshComp));
	RadialForceComp->Radius = 250;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false; // Prevent component from ticking, and only use FireImpulse() instead
	RadialForceComp->bIgnoreOwningActor = true; // ignore self


	SetReplicates(true);
	SetReplicateMovement(true);

	NetUpdateFrequency = 66.f;
	MinNetUpdateFrequency = 33.0f;
	

}

// Called when the game starts or when spawned, by server and all clients
void ASGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	// Hook this event on server and all clients
	OnDestroyed.AddDynamic(this, &ASGrenadeProjectile::MyOnDestroyed);

	// Only hook Explode timer on server
	if (GetLocalRole() == ROLE_Authority)
	{
		// Bind to overlap event on server because functionality needs to be run on server
		OnActorBeginOverlap.AddDynamic(this, &ASGrenadeProjectile::HandleBeginOverlap);

		GetWorldTimerManager().SetTimer(TimerHandle_TimeUntilExplode, this, &ASGrenadeProjectile::Explode, 1.f);
	}
	
}
// Timer for this function only setup as server, never called as client
void ASGrenadeProjectile::Explode()
{
	//UE_LOG(LogTemp, Warning, TEXT("Explode Called"));
	// Deal Damage, only do this on server
	UGameplayStatics::ApplyRadialDamage(this, 35.f, GetActorLocation(), 300.f, nullptr, TArray<AActor*>(), this, this->GetInstigatorController(), true, ECC_Visibility);
	// Blast away nearby physics actors, if they are being replicated then server can do this alone, else need to replicate this call
	RadialForceComp->FireImpulse();

	Destroy();
}


// This is bound to everyone, not replicated?
void ASGrenadeProjectile::MyOnDestroyed(AActor* DestroyedActor)
{
	// want to print the network owner somehow!
	//UE_LOG(LogTemp, Warning, TEXT("On Destroyed Called by %s"), *GetName());
	// If server has a player controller then spawn emitter?
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
}

// This is only called on server since it is bound to server on begin play
void ASGrenadeProjectile::HandleBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	Explode();
}




