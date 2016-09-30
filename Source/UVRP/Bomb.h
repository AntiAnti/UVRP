// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define BOMB_MAX_DAMAGE_RADIUS 300.0f

#include "Animation/SkeletalMeshActor.h"
#include "Bomb.generated.h"

class ASoldierBasic;

UCLASS(Blueprintable)
class UVRP_API ABomb : public ASkeletalMeshActor
{
	GENERATED_BODY()

public:
	ABomb();

	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Bomb")
	class USphereComponent* CollisionComp;
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Bomb")
	class USphereComponent* TriggerComp;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	USphereComponent* ExternalCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bomb")
	bool IsPassive;

	UPROPERTY(EditDefaultsOnly, Category = "Bomb")
	UParticleSystem* ExplosionEffect;

	UFUNCTION(Category = "Bomb")
	void SetState(bool NewState);

	UFUNCTION(Category = "Bomb")
	void SwitchState() { SetState(!bState); };

	UFUNCTION(Category = "Bomb")
	void Throw(FVector LinearSpeed);

	UFUNCTION(BlueprintNativeEvent, Category = "Bomb")
	void OnPickUp();
	virtual void OnPickUp_Implementation() {};

protected:
	UPROPERTY()
	bool bState;

	UPROPERTY()
	bool bIsAttached;

	UPROPERTY()
	UMaterialInstanceDynamic* mti_Light;

	UPROPERTY()
	UAnimSequence* AnimOpen;

	UPROPERTY()
	UAnimSequence* AnimClose;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

	UPROPERTY()
	TArray<ASoldierBasic*> AffectedSoldiers;

	UPROPERTY()
	bool DisableSoldierArray;

	UPROPERTY()
	FTimerHandle hActivateCollisionTimer;

	UFUNCTION()
	void OnTimer_ActivateCollision();

public:
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/*
	UFUNCTION()
	void OnHit1(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) { OnHit(OverlappedComponent, OtherActor, OtherComp, NormalImpulse, Hit); };
	*/

	UFUNCTION()
	void External_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Trigger_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Trigger_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
