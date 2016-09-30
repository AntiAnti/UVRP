// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TeleportBase.h"
#include "Teleport.generated.h"

UCLASS(Blueprintable)
class UVRP_API ATeleport : public AActor, public IMenuInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATeleport();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	bool OnClick(FVector HitPoint) override;

	bool IsUsingCustomSound() override;

public:
	/*
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global")
	class USkeletalMesh* TeleportLock;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global")
	class UStaticMesh* TeleportField;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global")
	class UBoxComponent* Box;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global")
	class UCapsuleComponent* TeleportTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Global")
	class UParticleSystem* FieldPS;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	bool IsLockActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	bool IsTeleportActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	int32 RingsFinished;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	bool IsReceivingPlayer;
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	ATeleportBase* TargetPlatformBase;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	int32 RingsStatus;

	UFUNCTION(BlueprintNativeEvent, Category = "Components")
	USkeletalMeshComponent* GetTeleportMesh();
	virtual USkeletalMeshComponent* GetTeleportMesh_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	UTeleportRingsSelector* RingSelector;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Class variables")
	bool IsLockOpen;

	UPROPERTY(BlueprintReadOnly, Category = "Class variables")
	float TeleportUnlockInterval;

	UPROPERTY(BlueprintReadOnly, Category = "Class variables")
	bool ShowFieldEffects;

	UPROPERTY(BlueprintReadWrite, Category = "Class variables")
	bool SelectorState;

	UFUNCTION(BlueprintCallable, Category = "Components")
	void ClearTargetPlatform() { TargetPlatformBase = nullptr; };

	UFUNCTION(BlueprintNativeEvent, Category = "Components")
	void OnLockClick(bool IsSuccessful);
	virtual void OnLockClick_Implementation(bool IsSuccessful) {};

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Components")
	void SetRingsStatusVar(int32 NewValue);
	virtual void SetRingsStatusVar_Implementation(int32 NewValue) { RingsStatus = NewValue; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Components")
	void SetLockOpenVar(bool NewValue);
	virtual void SetLockOpenVar_Implementation(bool NewValue) { IsLockOpen = NewValue; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class variables")
	FVector CurrentBaseLocation;
};
