// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Components/ActorComponent.h"
#include "SoliderVoiceReactComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UVRP_API USoliderVoiceReactComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoliderVoiceReactComponent();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	UPROPERTY()
	FTimerHandle TimerCheckReaction;
	
	UFUNCTION()
	void CheckReaction();

	/*
	UFUNCTION(BlueprintNativeEvent, Category = "Callback")
	void Speak(EVoiceQuote quote);
	virtual void Speak_Implementation(EVoiceQuote quote);
	*/
};
