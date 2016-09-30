// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Teleport.h"
#include "UVRPGameMode.h"
#include "TeleportBase.h"


// Sets default values
ATeleportBase::ATeleportBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//RoomCentreLocation = GetActorLocation();
}

// Called when the game starts or when spawned
void ATeleportBase::BeginPlay()
{
	Super::BeginPlay();
	
	ActiveGameMode = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}

// Called every frame
void ATeleportBase::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

ATeleport* ATeleportBase::GetTeleport_Implementation()
{
	return nullptr;
}

FVector ATeleportBase::GetPlayerCentreLocation()
{
	FVector vRet;

	FVector vOffset = FVector::ZeroVector;
	if (!IsValid(ActiveGameMode)) ActiveGameMode = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (IsValid(ActiveGameMode)) {
		vOffset.Z = ActiveGameMode->GO_FloorOffset;
	}

	if (RoomCentreLocation == FVector::ZeroVector)
		vRet = GetActorLocation() + FVector(0.0f, 0.0f, 80.0f /*74.0f*/) + vOffset;
	else
		vRet = RoomCentreLocation + vOffset;

	return vRet;
}
