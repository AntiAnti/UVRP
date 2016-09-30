// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "SoliderVoiceReactComponent.h"


// Sets default values for this component's properties
USoliderVoiceReactComponent::USoliderVoiceReactComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void USoliderVoiceReactComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//GetWorld()->GetTimerManager().SetTimer(TimerCheckReaction, &USoliderVoiceReactComponent::CheckReaction, 0.5f, true, -1.0f);
	const FTimerDelegate FireDelegate = FTimerDelegate::CreateUObject(this, &USoliderVoiceReactComponent::CheckReaction);
	const float Interval = 0.5f;

	GetOwner()->GetWorldTimerManager().SetTimer(TimerCheckReaction, FireDelegate, Interval, true);
}


// Called every frame
void USoliderVoiceReactComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	// ...
}
void USoliderVoiceReactComponent::CheckReaction()
{
}

/*
void USoliderVoiceReactComponent::Speak_Implementation(EVoiceQuote quote)
{
}

*/