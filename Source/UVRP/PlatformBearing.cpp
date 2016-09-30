// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "PlatformBearing.h"
#include "EngineUtils.h"


// Sets default values
APlatformBearing::APlatformBearing()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MechanismMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MechanismMesh"));
	SetRootComponent(MechanismMesh);
	MechanismMesh->RelativeLocation = FVector(0.f, 0.f, 00.f);
	MechanismMesh->bCastDynamicShadow = false;
	MechanismMesh->CastShadow = true;

	IsActive = false;
}

// Called when the game starts or when spawned
void APlatformBearing::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlatformBearing::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool APlatformBearing::GetState()
{
	return IsActive;
}

void APlatformBearing::Activate()
{
	IsActive = true;
}

void APlatformBearing::DetachPlatformActor()
{
	if (IsValid(AttachedPlatformPointer)) {
		AttachedPlatformPointer->DetachRootComponentFromParent(true);
	}
}

void APlatformBearing::ResetState()
{
	IsActive = false;
}