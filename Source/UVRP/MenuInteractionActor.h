// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MenuInteractionActor.generated.h"

UCLASS(Blueprintable)
class UVRP_API AMenuInteractionActor : public AActor, public IMenuInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMenuInteractionActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	UFUNCTION()
	bool OnClick(FVector HitPoint) override;

public:
	UFUNCTION(BlueprintNativeEvent, Category = "MenuInteractionInterface")
	bool OnMenuClickEvent(FVector HitPoint);
	virtual bool OnMenuClickEvent_Implementation(FVector HitPoint);
};
