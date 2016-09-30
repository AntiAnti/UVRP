// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "GamePlatform.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"

// Sets default values
AGamePlatform::AGamePlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	for (int i = 0; i < 4; i++) {
		LocationSoldiers[i] = "";
		ShieldsActivated[i] = false;
	}

	ShieldState1 = false;
	ShieldState2 = false;
	ShieldState3 = false;
	ShieldState4 = false;

	IsBackdoorTeleport = false;
	IsPlatformAvalibleForActivation = false;
	IsTrainingLevelPlatform = false;
	NextPlatforms.Empty();
	PlatformRadius = 120.0f;
	IsPlayerCanReturnToThePlatform = false;
}

// Called when the game starts or when spawned
void AGamePlatform::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGamePlatform::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );

}

FVector AGamePlatform::GetProtectedStayLocationC(bool AgainstPlayer, FString SoldierName)
{
	FVector vReturn = GetActorLocation();
	float CurrDist = 10000.0f;
	int CurrIndex = -1;
	float NewDist;
	vReturn.Z = 36.0f;

	TArray<FVector> AvalibleLocations;
	TArray<int32> ActiveShields;

	for (int i = 0; i < 4; i++) {
		AvalibleLocations.Add(GetSafeLocationC(i+1));
	}

	if (ShieldState1) ActiveShields.Add(0);
	if (ShieldState2) ActiveShields.Add(1);
	if (ShieldState3) ActiveShields.Add(2);
	if (ShieldState4) ActiveShields.Add(3);

	if (!AgainstPlayer) {
		if (ActiveShields.Num() > 0)
			vReturn = AvalibleLocations[ActiveShields[0]];
		return vReturn;
	}

	if (ActiveShields.Num() > 0) {
		const APawn *Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		FVector PointFrom = Pawn->GetActorLocation();

		for (int i = 0; i < ActiveShields.Num(); i++) {
			if (LocationSoldiers[ActiveShields[i]] != SoldierName && LocationSoldiers[ActiveShields[i]] != "") continue;

			FVector res = PointFrom - AvalibleLocations[ActiveShields[i]];
			NewDist = FMath::Sqrt(res.X*res.X + res.Y*res.Y + res.Z*res.Z);

			if (NewDist < CurrDist) {
				CurrDist = NewDist;
				CurrIndex = ActiveShields[i];
			}
		}

		if (CurrIndex >= 0) {
			vReturn = AvalibleLocations[CurrIndex];

			for (int i = 0; i < 4; i++) {
				if (i == CurrIndex)
					LocationSoldiers[i] = SoldierName;
				else if (LocationSoldiers[i] == SoldierName)
					LocationSoldiers[i] = "";
			}
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, FString::FromInt(CurrIndex));

	return vReturn;
}

FVector AGamePlatform::GetNearbyLocation(FVector InitLocation, int32 Sector, int32 SectorsCount, float HeightOffset, float HeightDeviation)
{
	float yaw, heig, dist, a, b;

	a = 2 * PI * (float)(Sector-1) / (float)SectorsCount;
	b = 2 * PI * (float)Sector / (float)SectorsCount;
	yaw = FMath::FRandRange(a, b);
	heig = FMath::FRandRange(HeightOffset - HeightDeviation, HeightOffset + HeightDeviation);
	dist = 4.0f * FMath::FRandRange(HeightOffset - HeightDeviation, HeightOffset + HeightDeviation);

	InitLocation.X += FMath::Cos(yaw) * dist;
	InitLocation.Y += FMath::Sin(yaw) * dist;
	InitLocation.Z += heig;

	return InitLocation;
}

void AGamePlatform::GenerateFuturePlatforms(int32 Depth, int32 BranchLevel)
{
	// https://pp.vk.me/c625616/v625616122/491ef/VVhOURGozd8.jpg
	ChechPlatformToSpawning(Depth, this, BranchLevel);
}

void AGamePlatform::ChechPlatformToSpawning(int32 Depth, AGamePlatform* Platform, int32 BranchLevel)
{
	if (Platform == nullptr) return;

	if (Platform->NextPlatforms.Num() == 0) {
		// нет следующих платформ
		for (int i = 1; i <= BranchLevel; i++) {
			AGamePlatform* nx = SpawnPlatformAtLocation(GetNearbyLocation(Platform->GetActorLocation(), i, BranchLevel));
			if (nx != nullptr) Platform->NextPlatforms.Add(nx);
		}
	}

	// пройтись по всем платформам
	if (Depth > 0)
		for (int i = 0; i < Platform->NextPlatforms.Num(); i++)
			ChechPlatformToSpawning(Depth - 1, Platform->NextPlatforms[i], BranchLevel);

	return;
}

AGamePlatform* AGamePlatform::SpawnPlatformAtLocation_Implementation(FVector Location)
{
	//
	return nullptr;
}

void AGamePlatform::ActivateFuturePlatforms()
{
	for (int i = 0; i < NextPlatforms.Num(); i++)
		NextPlatforms[i]->IsPlatformAvalibleForActivation = true;
}

void AGamePlatform::LoadPlatformsGuardians(AGamePlatform *FirstPlatform, FString LastPlatformName)
{
	LoadPlatformsGuardians_R(FirstPlatform, LastPlatformName);
}

FVector AGamePlatform::GetNearestBorderPoint(FVector CurrentLocation)
{
	FVector vRet;
	FVector loc, cloc, centre;
	float radius = PlatformRadius - 35.0f;
	float dist = 300.0f, cdist;
	const float ShieldExtend = 18.0f;
	centre = GetActorLocation();

	// find nearest shield
	for (int i = 1; i <= 4; i++) {
		cloc = GetShieldLocation(i);
		if (cloc == FVector::ZeroVector) continue;

		cdist = (cloc - CurrentLocation).Size();

		if (cdist < dist) {
			loc = cloc; dist = cdist;
		}
	}

	// dont have shields
	if (dist == 300.0f) {
		vRet = (CurrentLocation - centre);
		vRet.Z = 0.0f;
		vRet.Normalize();
		vRet = centre + vRet * radius;
	}
	else {
		// find point near shield border
		FVector p1, p2;
		cloc = loc - centre;
		float angle = FMath::Atan2(cloc.Y, cloc.X);
		p1 = centre + FVector(FMath::Cos(angle - ShieldExtend) * radius, FMath::Sin(angle - ShieldExtend) * radius, 0.0f);
		p2 = centre + FVector(FMath::Cos(angle + ShieldExtend) * radius, FMath::Sin(angle + ShieldExtend) * radius, 0.0f);

		if ((p2 - CurrentLocation).Size() < (p1 - CurrentLocation).Size())
			vRet = p2;
		else
			vRet = p1;

		cloc.Normalize();
	}
	//DrawDebugLine(GetWorld(), vRet, vRet + FVector(0.0f, 0.0f, 200.0f), FColor::White, false, 2.0f, 0, 9.0f);

	return vRet;
}

void AGamePlatform::LoadPlatformsGuardians_R(AGamePlatform *CurrentPlatform, FString LastPlatformName)
{
	CurrentPlatform->GuardiansData.Empty();

	if (CurrentPlatform->GetName() == LastPlatformName)
		return;
	else {
		for (int32 i = 0; i < CurrentPlatform->NextPlatforms.Num(); i++)
			LoadPlatformsGuardians_R(CurrentPlatform->NextPlatforms[i], LastPlatformName);
	}
}

void AGamePlatform::SetupTeleportAtLoading_Implementation(AGamePlatform *TargetPlatform)
{
	//
}

AGamePlatform* AGamePlatform::GetTeleportTargetPlatform_Implementation()
{
	return nullptr;
}

FVector AGamePlatform::GetSafeLocationC_Implementation(int32 ShieldNumber)
{
	FVector ret = FVector::ZeroVector;
	switch (ShieldNumber) {
	case 1:
		ret = FVector(0.0f, -115.0f, 36.0f); break;
	case 2:
		ret = FVector(115.0f, 0.0f, 36.0f); break;
	case 3:
		ret = FVector(0.0f, 115.0f, 36.0f); break;
	case 4:
		ret = FVector(-115.0f, 0.0f, 36.0f); break;
	}
	ret += GetActorLocation();

	return ret;
}