// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LevelChangableInteractionActor.generated.h"

UCLASS(Blueprintable)
class UVRP_API ALevelChangableInteractionActor :	public AActor,
													public IMenuInteractionInterface,
													public ISaveObjectInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelChangableInteractionActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Save Object Interface")
	int32 ObjectNumber;

private:
	UFUNCTION()
	bool OnClick(FVector HitPoint) override;

	/*-------------------------------------------------------*/

	UFUNCTION()
	int32 GetNumber() override;
	UFUNCTION()
	bool IsExists() override;
	UFUNCTION()
	bool IsExpansionStarted() override;
	UFUNCTION()
	bool IsExpansionFinished() override;
	UFUNCTION()
	void SetExpansionStarted() override;
	UFUNCTION()
	void SetExpansionFinished() override;
	UFUNCTION()
	bool IsActive() override;
	UFUNCTION()
	void SetActive() override;
	UFUNCTION()
	FTransform GetTransform() override;
	UFUNCTION()
	void SetTransform(FTransform NewTransform) override;
	UFUNCTION()
	void DestroyActor() override;

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Menu Interaction Interface")
	bool OnMenuClickEvent(FVector HitPoint);
	virtual bool OnMenuClickEvent_Implementation(FVector HitPoint);

	/*-------------------------------------------------------*/

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	bool IsSaveObjectExists();
	virtual bool IsSaveObjectExists_Implementation() { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	bool IsSaveObjectExpansionStarted();
	virtual bool IsSaveObjectExpansionStarted_Implementation() { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	bool IsSaveObjectExpansionFinished();
	virtual bool IsSaveObjectExpansionFinished_Implementation() { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	void SetSaveObjectExpansionStarted();
	virtual void SetSaveObjectExpansionStarted_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	void SetSaveObjectExpansionFinished();
	virtual void SetSaveObjectExpansionFinished_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	bool IsSaveObjectActive();
	virtual bool IsSaveObjectActive_Implementation() { return false; };

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	void SetSaveObjectActive();
	virtual void SetSaveObjectActive_Implementation() {};

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	FTransform GetSaveObjectTransform();
	virtual FTransform GetSaveObjectTransform_Implementation() { return FTransform(); };

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	void SetSaveObjectTransform(FTransform NewTransform);
	virtual void SetSaveObjectTransform_Implementation(FTransform NewTransform) {};

	UFUNCTION(BlueprintNativeEvent, Category = "Save Object Interface")
	void DestroySavedActor();
	virtual void DestroySavedActor_Implementation() { Destroy(); };
};