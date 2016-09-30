// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MenuCollisionBox.generated.h"

UCLASS(Blueprintable)
class UVRP_API AMenuCollisionBox : public AActor /*, public IMenuInteractionInterface*/
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMenuCollisionBox();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default")
	int32 TargetTerminalLevel;

public:
	UFUNCTION(BlueprintCallable, Category = "Callback")
	void SetTerminalLevel();

	UFUNCTION(BlueprintNativeEvent, Category = "Callback")
	void OnBoxClicked(int32 Level);
	virtual void OnBoxClicked_Implementation(int32 Level);
};
