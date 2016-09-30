// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define BOMB_MAX_DAMAGE_RADIUS 200.0f

#include "ThrowProjectileBase.h"
#include "ProjectileBomb.generated.h"

class ASoldierBasic;
class UGunSlingComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API AProjectileBomb : public AThrowProjectileBase
{
	GENERATED_BODY()

public:
	AProjectileBomb();
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UStaticMeshComponent* BombMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USphereComponent* ExternalCollisionComp;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* ExplosionEffect;

public:
	void ActivateProjectile(USceneComponent* AttachParent, FVector AngularImpulse, FVector LinearImpulse) override;

	void SetVisibility(bool NewVisibility) override;

protected:
	UPROPERTY()
	TArray<ASoldierBasic*> AffectedSoldiers;
	UPROPERTY()
	float fLifeSpanAfterFire;

public:
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void External_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void External_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};