// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ThrowProjectileBase.h"

// Sets default values
AThrowProjectileBase::AThrowProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(CollisionComp);
	CollisionComp->InitSphereRadius(0.2f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AThrowProjectileBase::OnHit);
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	//CollisionComp->SetMassScale(NAME_None, 0.5f);
	//CollisionComp->SetMassOverrideInKg(NAME_None, 0.1f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	//ProjectileMovement->Velocity = FVector(1.0f, 0.0f, 0.0f);
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bAutoActivate = false;
		
	Damage = 1.0f;
}

// Called when the game starts or when spawned
void AThrowProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThrowProjectileBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

