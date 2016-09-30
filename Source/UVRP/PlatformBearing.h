// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GamePlatform.h"
#include "PlatformBearing.generated.h"

UCLASS(Blueprintable)
class UVRP_API APlatformBearing : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlatformBearing();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Meta = (ExposeFunctionCategories = "Mesh,Components|SkeletalMesh,Animation,Physics", AllowPrivateAccess = "true"))
	class USkeletalMeshComponent* MechanismMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AGamePlatform* AttachedPlatformPointer;
	
	UFUNCTION(BlueprintCallable, Category = "State Control")
	bool GetState();

	UFUNCTION(BlueprintCallable, Category = "State Control")
	void ResetState();

	UFUNCTION(BlueprintCallable, Category = "State Control")
	void Activate();

	UFUNCTION(BlueprintCallable, Category = "State Control")
	void DetachPlatformActor();

protected:
	UPROPERTY()
	bool IsActive;
};
