// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "UVRP.h"
#include "UVRPProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

AUVRPProjectile::AUVRPProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	// Set as root component
	SetRootComponent(CollisionComp);

	CollisionComp->InitSphereRadius(0.2f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AUVRPProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;


	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	psTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("psTrail"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> peTrailSystem(TEXT("ParticleSystem'/Game/FirstPerson/Effects/ps_BulletTrail.ps_BulletTrail'"));
	psTrail->SetupAttachment(GetRootComponent());
	if (peTrailSystem.Object != NULL)
		psTrail->SetTemplate(peTrailSystem.Object);
	psTrail->SetRelativeLocation(FVector(-34.58f, 0.0f, 0.0f));
	psTrail->SetRelativeRotation(FRotator::ZeroRotator);
	psTrail->bAutoActivate = true;
	psTrail->bResetOnDetach = true;
	psTrail->SetVisibility(true);

	// Die after 2 seconds by default
	InitialLifeSpan = 2.0f;
}

void AUVRPProjectile::SetBulletTrailVisibility(bool IsVisible)
{
	if (!IsVisible) {
		psTrail->bAutoActivate = IsVisible;
		if (IsVisible)
			psTrail->Activate(true);
		else
			psTrail->Deactivate();

		psTrail->SetVisibility(IsVisible);
	}
}

void AUVRPProjectile::OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		//ProjectileMovement->MaxSpeed = 0.0f;
		//Destroy();
		HitsCount++;
	}
}