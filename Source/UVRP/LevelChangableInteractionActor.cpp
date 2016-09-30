// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "MenuInteractionInterface.h"
#include "SaveObjectInterface.h"
#include "LevelChangableInteractionActor.h"


// Sets default values
ALevelChangableInteractionActor::ALevelChangableInteractionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ALevelChangableInteractionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelChangableInteractionActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ALevelChangableInteractionActor::OnClick(FVector HitPoint)
{
	return OnMenuClickEvent(HitPoint);
}

bool ALevelChangableInteractionActor::OnMenuClickEvent_Implementation(FVector HitPoint)
{
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int32 ALevelChangableInteractionActor::GetNumber()
{
	return ObjectNumber;
}

bool ALevelChangableInteractionActor::IsExists()
{
	return IsSaveObjectExists();
}

bool ALevelChangableInteractionActor::IsExpansionStarted()
{
	return IsSaveObjectExpansionStarted();
}

bool ALevelChangableInteractionActor::IsExpansionFinished()
{
	return IsSaveObjectExpansionFinished();
}

void ALevelChangableInteractionActor::SetExpansionStarted()
{
	SetSaveObjectExpansionStarted();
}

void ALevelChangableInteractionActor::SetExpansionFinished()
{
	SetSaveObjectExpansionFinished();
}

bool ALevelChangableInteractionActor::IsActive()
{
	return IsSaveObjectActive();
}

void ALevelChangableInteractionActor::SetActive()
{
	SetSaveObjectActive();
}

FTransform ALevelChangableInteractionActor::GetTransform()
{
	return GetSaveObjectTransform();
}

void ALevelChangableInteractionActor::SetTransform(FTransform NewTransform)
{
	SetSaveObjectTransform(NewTransform);
}

void ALevelChangableInteractionActor::DestroyActor()
{
	DestroySavedActor();
}
