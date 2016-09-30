// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define SOCKET_ProjectileBase	"ProjectileBaseSocket"
#define SOCKET_Fire				"FireSocket"
#define PULL_MinForce			400.0f
#define PULL_WorkForce			410.0f
#define PULL_MaxForce			900.0f

#include "Components/SkeletalMeshComponent.h"
#include "UVRPGameMode.h"
#include "GunSlingComponent.generated.h"

class AThrowProjectileBase;
class AUVRPCharacter;

UENUM(BlueprintType)
enum class EHeavyGunState : uint8
{
	C_Bomb			UMETA(DisplayName = "Bomb"),
	C_Sling			UMETA(DisplayName = "Sling")
};


UCLASS(Blueprintable)
class UVRP_API UGunSlingComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

public:
	UGunSlingComponent();
	virtual void BeginPlay() override;

	UPROPERTY()
	TSubclassOf<AThrowProjectileBase> Class_ProjectileBomb;

	UPROPERTY()
	TSubclassOf<AThrowProjectileBase> Class_ProjectileSling;

public:
	UFUNCTION()
	void ChangeGunState(EHeavyGunState NewState, bool SwapModes = false);

	UFUNCTION()
	void Fire(FVector LinearVelocity);

	UFUNCTION()
	void SetWithChildsVisibility(bool NewVisibility);

	UFUNCTION()
	bool HasTug();

	UFUNCTION()
	FVector GetTugLocation();

	UFUNCTION()
	void TranslatePullForce(FVector Direction, float Force);

	UFUNCTION()
	void ReleaseProjectile(); // Keep it alive white it's moving

	UFUNCTION()
	void DestroyProjectile(); // Destroy immediately

protected:
	UPROPERTY()
	EHeavyGunState GunState;
	UPROPERTY()
	UAnimSequence* Anim_OpenState1;
	UPROPERTY()
	AThrowProjectileBase* CurrentProjectile;
	UPROPERTY()
	AUVRPCharacter* Player;
	UPROPERTY()
	FHandFeedbackSetup FeedbackWave;
	UPROPERTY()
	bool IsRightController;

	UPROPERTY()
	USoundBase* ShieldDetachSound;

	UFUNCTION()
	void SpawnProjectile();
};
