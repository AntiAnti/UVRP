// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "MenuInteractionInterface.h"
#include "MenuInteractionActor.h"


// Sets default values
AMenuInteractionActor::AMenuInteractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMenuInteractionActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMenuInteractionActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool AMenuInteractionActor::OnClick(FVector HitPoint)
{
	return OnMenuClickEvent(HitPoint);
}

bool AMenuInteractionActor::OnMenuClickEvent_Implementation(FVector HitPoint)
{
	return false;
}

