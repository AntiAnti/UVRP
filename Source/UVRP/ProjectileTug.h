// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "ThrowProjectileBase.h"
#include "ProjectileTug.generated.h"

class AGamePlatform;
class UGunSlingComponent;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UVRP_API AProjectileTug : public AThrowProjectileBase
{
	GENERATED_BODY()

public:
	AProjectileTug();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY()
	USkeletalMeshComponent *BombMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystemComponent* BeamLightning;

private:
	UPROPERTY()
	UAnimSequence* AnimOpen;

	UPROPERTY()
	UPrimitiveComponent* AttachedShield;
	UPROPERTY()
	AGamePlatform* AttachedPlatform;

	UPROPERTY()
	FTimerHandle hCheckBeamTimer;
	UPROPERTY()
	float fLifeSpanAfterFire;
	UPROPERTY()
	bool bCanBePulled;

	UPROPERTY()
	float StartWavingTime;
	UPROPERTY()
	float CurrentWavePos;
	UPROPERTY()
	float CurrentWaveValue;
	UPROPERTY()
	float WaveLength;
	UPROPERTY()
	FHandFeedbackSetup FeedbackWave;
	UPROPERTY()
	AUVRPCharacter* PlayerOwner;
	UPROPERTY()
	bool IsRightController;
	UPROPERTY()
	float TimeInFlight;

	UFUNCTION()
	void OnDestroyProjectile();
	UFUNCTION()
	void TimerKillBeam();
	UFUNCTION()
	void TimerCheckBeam();

public:
	UFUNCTION()
	void InitializeProjectileFeedback(AUVRPCharacter* Player, bool IsRightHand, FHandFeedbackSetup OptFeedbackWave);

	UFUNCTION()
	void ActivateProjectile(USceneComponent* AttachParent, FVector AngularImpulse, FVector LinearImpulse) override;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void SetVisibility(bool NewVisibility) override;

	void InitializeProjectile(USceneComponent* ParentGun) override;

	UFUNCTION()
	bool IsDetached() override;

	UFUNCTION()
	bool ForcePull(FVector Direction, float Force) override;
};
