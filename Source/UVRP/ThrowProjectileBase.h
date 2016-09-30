// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ThrowProjectileBase.generated.h"

class UGunSlingComponent;

UCLASS()
class UVRP_API AThrowProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AThrowProjectileBase();

	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY()
	float Damage;

	UFUNCTION()
	virtual void InitializeProjectile(USceneComponent* ParentGun) {};

	UFUNCTION()
	void DestroyProjectile() { Destroy(); };

	UFUNCTION()
	virtual void SetVisibility(bool NewVisibility) {};

public:
	UFUNCTION()
	virtual bool IsDetached() { return false; };

	UFUNCTION()
	virtual bool ForcePull(FVector Direction, float Force) { return false; };

	UFUNCTION()
	void InitParentGun(UGunSlingComponent* GunComponent) { ParentGun = GunComponent; };

	UFUNCTION()
	virtual void OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {};

	UFUNCTION()
	virtual void ActivateProjectile(USceneComponent* AttachParent, FVector AngularImpulse, FVector LinearImpulse) {};

	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	UPROPERTY()
	UGunSlingComponent* ParentGun;
};
