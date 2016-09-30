// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "SoldierBasic.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileBomb.h"

AProjectileBomb::AProjectileBomb()
{
	PrimaryActorTick.bCanEverTick = true;

	BombMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombMesh"));
	BombMesh->SetupAttachment(CollisionComp);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAsset(TEXT("StaticMesh'/Game/FirstPerson/Objects/GunSling/sm_ProjectileBomb.sm_ProjectileBomb'"));
	if (smAsset.Object != NULL) BombMesh->SetStaticMesh(smAsset.Object);
	BombMesh->SetMobility(EComponentMobility::Movable);
	//BombMesh->RegisterComponent();
	BombMesh->SetRelativeLocation(FVector::ZeroVector);
	CollisionComp->SetSimulatePhysics(false);

	ExternalCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("ExternalCollisionComp"));
	ExternalCollisionComp->SetupAttachment(CollisionComp);
	ExternalCollisionComp->SetRelativeLocation(FVector::ZeroVector);
	ExternalCollisionComp->InitSphereRadius(BOMB_MAX_DAMAGE_RADIUS);
	ExternalCollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ExternalCollisionComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	ExternalCollisionComp->CanCharacterStepUpOn = ECB_No;
	ExternalCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ExternalCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AProjectileBomb::External_OnBeginOverlap);
	ExternalCollisionComp->OnComponentEndOverlap.AddDynamic(this, &AProjectileBomb::External_OnEndOverlap);
	//ExternalCollisionComp->RegisterComponent();

	static ConstructorHelpers::FObjectFinder<UParticleSystem> psExAsset(TEXT("ParticleSystem'/Game/FirstPerson/Effects/P_Pixel_Explosion_Player.P_Pixel_Explosion_Player'"));
	if (psExAsset.Object != NULL) ExplosionEffect = psExAsset.Object;

	fLifeSpanAfterFire = 10.0f;
}

void AProjectileBomb::BeginPlay()
{
	Super::BeginPlay();

	/*
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	*/
	CollisionComp->IgnoreActorWhenMoving(GetWorld()->GetFirstPlayerController()->GetPawn(), true);
}

void AProjectileBomb::External_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ASoldierBasic::StaticClass())) {
		AffectedSoldiers.Add(Cast<ASoldierBasic>(OtherActor));
	}
}

void AProjectileBomb::External_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ASoldierBasic::StaticClass())) {
		for (int i = 0; i < AffectedSoldiers.Num(); i++) {
			if (AffectedSoldiers[i]->GetUniqueID() == OtherActor->GetUniqueID()) {
				FVector MyLoc = GetActorLocation();
				FVector vDir = AffectedSoldiers[i]->GetActorLocation() - MyLoc;
				float dist = vDir.Size();
				if (dist > BOMB_MAX_DAMAGE_RADIUS) dist = BOMB_MAX_DAMAGE_RADIUS;

				vDir.Normalize();
				AffectedSoldiers[i]->OnSoftDamage(Damage * (1 - dist / BOMB_MAX_DAMAGE_RADIUS) /* linear - for now! */, MyLoc, vDir);

				AffectedSoldiers.RemoveAt(i);
				break;
			}
		}
	}
}

void AProjectileBomb::ActivateProjectile(USceneComponent* AttachParent, FVector AngularImpulse, FVector LinearImpulse)
{
	Super::ActivateProjectile(AttachParent, AngularImpulse, LinearImpulse);

	SetActorTransform(AttachParent->GetSocketTransform("FireSocket"));
	DetachRootComponentFromParent(true);


	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExternalCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileMovement->SetVelocityInLocalSpace(FVector(LinearImpulse.Size(), 0.0f, 0.0f));
	//ProjectileMovement->Velocity = GetActorRotation().Vector() * LinearImpulse.Size();
	ProjectileMovement->Velocity = LinearImpulse;
	ProjectileMovement->InitialSpeed = LinearImpulse.Size();
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->SetActive(true);
	ProjectileMovement->Activate();

	/*
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetSimulatePhysics(true);
	CollisionComp->AddImpulse(LinearImpulse);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LinearImpulse * 100.0f, FColor::White, false, 2.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() - LinearImpulse, FColor::Blue, false, 2.0f);
	*/

	SetLifeSpan(fLifeSpanAfterFire);
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::SanitizeFloat(LinearImpulse.Size()));
}

void AProjectileBomb::OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit);

	FVector MyLoc = GetActorLocation();
	FVector vDir;
	float dist;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, MyLoc);

	for (int i = 0; i < AffectedSoldiers.Num(); i++) {
		FVector MyLoc = GetActorLocation();
		FVector vDir = AffectedSoldiers[i]->GetActorLocation() - MyLoc;
		dist = vDir.Size();
		if (dist > BOMB_MAX_DAMAGE_RADIUS) dist = BOMB_MAX_DAMAGE_RADIUS;

		vDir.Normalize();
		AffectedSoldiers[i]->OnSoftDamage(Damage * (1 - dist / BOMB_MAX_DAMAGE_RADIUS) /* linear - for now! */, MyLoc, vDir);
	}
	DestroyProjectile();
}

void AProjectileBomb::SetVisibility(bool NewVisibility)
{
	Super::SetVisibility(NewVisibility);

	BombMesh->SetVisibility(NewVisibility);
}