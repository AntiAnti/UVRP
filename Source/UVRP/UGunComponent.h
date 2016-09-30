// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/StaticMeshComponent.h"
#include "UGunComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API UUGunComponent : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	// Sets default values for this component's properties
	UUGunComponent();

	// Called when the game starts
	virtual void BeginPlay() override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AUVRPProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	USoundBase *FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	bool IsAiming;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	bool AutoFiring;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	float ShootingSpeedInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	float ShotTemperatureIncrement;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	float ShootingMinimumInterval;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	float CoolingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	float CoolDownTemp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Global)
	bool UsedByPlayer;

	UPROPERTY(BlueprintReadOnly, Category = Global)
	bool UpdateMaterialEmission;

	UPROPERTY(BlueprintReadOnly, Category = Global)
	bool ShowBulletTrail;

	UFUNCTION(BlueprintCallable, Category = Manual)
	bool Fire();

	UFUNCTION(BlueprintCallable, Category = Global)
	float GetTemperature();

	UFUNCTION(BlueprintCallable, Category = Global)
	float GetWorkingTemperature();

	UFUNCTION(BlueprintCallable, Category = Global)
	bool IsGunCanShot();

	UFUNCTION(BlueprintCallable, Category = Shooting)
	bool TraceToPoint(FVector TargetLocation);

	UFUNCTION(BlueprintCallable, Category = Global)
	void SetAutoFiring(bool NewValue);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = State)
	float fTemperature;	

	inline FVector GetGunFiringPoint();
	FRotator FindProjectileDirection(float Dispersion = 0.1f);

	UPROPERTY()
	USoundAttenuation *SoundEffect;

	UPROPERTY()
	FTimerHandle hCooldownTimer;

	UPROPERTY()
	UMaterialInstanceDynamic* mti_Main;

	UFUNCTION()
	void TimerCooldown_Tick();

private:
	UPROPERTY()
	float fCumulatedInterval;
	UPROPERTY()
	bool bRestoringTemperature;
	UPROPERTY()
	float PrevEmissionParamValue;
};
