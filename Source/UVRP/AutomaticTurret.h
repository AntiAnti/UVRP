// Fill out your copyright notice in the Description page of Project Settings.

#define _TURRET_STATE_DISABLED 0
#define _TURRET_STATE_AIMING_PAWN 1
#define _TURRET_STATE_AIMING_CHARGER 2
#define _TURRET_STATE_SHOOTING 3
#define _TURRET_STATE_CHARGING 4
#define _TURRET_STATE_PREPAREGUN 5

#pragma once

#include "Animation/SkeletalMeshActor.h"
#include "AutomaticTurret.generated.h"

class AUVRPCharacter;
class AGamePlatform;

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API AAutomaticTurret : public ASkeletalMeshActor
{
	GENERATED_BODY()

public:
	AAutomaticTurret();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class AGamePlatform* ParentPlatform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UBoxComponent* KillingCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetingHorizontalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetingVerticalSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootingDamagePower; /* points per second */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ChargingInterval; /* 0 to 1 charge time, sec */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ShootingDischargeInterval; /* 1 to 0 discharge time, sec */

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GunHeatingInterval; /* time b/w aiming and shooting */

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HitDamage;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* LaserBeam;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystemComponent* LaserTargetingBeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

public:
	UFUNCTION(BlueprintCallable, Category = "Global")
	void Activate();

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool IsActive() { return bIsActive; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	float GetRotationHorizontal() {	return CurrentRotationHorizontal; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	float GetRotationVertical() { return CurrentRotationVertical; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	float GetCharge() { return fCharge; };	

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void AimingFinished();
	virtual void AimingFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void AimingStarted();
	virtual void AimingStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ShootingPreparingStarted();
	virtual void ShootingPreparingStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ShootingPreparingFinished();
	virtual void ShootingPreparingFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ShootingBeamStarted();
	virtual void ShootingBeamStarted_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void ShootingBeamFinished();
	virtual void ShootingBeamFinished_Implementation();

	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnTurretDestroyed();
	virtual void OnTurretDestroyed_Implementation();

private:
	UPROPERTY()
	bool bIsActive;

	UPROPERTY()
	float CurrentRotationHorizontal;

	UPROPERTY()
	float CurrentRotationVertical;

	UPROPERTY()
	float TargetRotationHorizontal;

	UPROPERTY()
	float TargetRotationVertical;

	UPROPERTY()
	AUVRPCharacter* TurretTarget;

	UPROPERTY()
	FVector CurrentShootingTarget;

	UPROPERTY()
	float fCharge;

	UPROPERTY()
	int32 nCurrentState;

	UPROPERTY()
	float CurrentEventLength;

protected:
	UFUNCTION()
	FVector GetShootingTarget();

	UFUNCTION()
	bool TraceToPoint(FVector SourceLocation, FVector TargetLocation);

	UFUNCTION()
	bool CheckCurrentShootingTarget();

	UFUNCTION()
	void CheckTargetForDamage(float DeltaTime);

public:
	UFUNCTION()
	void OnPlayerHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
