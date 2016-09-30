// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "TMovingActor.generated.h"

UCLASS(Blueprintable)
class UVRP_API ATMovingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATMovingActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	float MovingSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving")
	bool bUndestroyable;
};
