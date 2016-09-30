// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "TeleportRingsSelector.h"

UTeleportRingsSelector::UTeleportRingsSelector()
{
	CurrentRing = -1;
	PrimaryComponentTick.bCanEverTick = false;
}

void UTeleportRingsSelector::BeginPlay()
{
	Super::BeginPlay();
	SelectorValue = false;
	if (!IsValid(mti_Main))
		mti_Main = this->CreateDynamicMaterialInstance(0);
}

void UTeleportRingsSelector::SetCurrentTeleportMesh(USkeletalMeshComponent *TeleportMesh)
{
	CurrentTeleportMesh = TeleportMesh;
}

void UTeleportRingsSelector::SetRing(int32 RingNumber)
{
	CurrentRing = RingNumber;
	if (IsValid(CurrentTeleportMesh))
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "CurrentTeleportMesh = (" + CurrentTeleportMesh->GetName() + ")");

	if (IsValid(CurrentTeleportMesh)) {
		switch (CurrentRing) {
			case 1: AttachToComponent(CurrentTeleportMesh, FAttachmentTransformRules::KeepRelativeTransform, "Ring1_Socket_Static"); break;
			case 2: AttachToComponent(CurrentTeleportMesh, FAttachmentTransformRules::KeepRelativeTransform, "Ring2_Socket_Static"); break;
			case 3: AttachToComponent(CurrentTeleportMesh, FAttachmentTransformRules::KeepRelativeTransform, "Ring3_Socket_Static"); break;
		}

		RelativeLocationToSocket = FVector(/*1.0f*/ 0.0f, 0.0f, 0.0f);
		SetRelativeLocation(RelativeLocationToSocket);
		SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f).Quaternion());
		SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "UTeleportRingsSelector::SetRing / CurrentTeleportMesh is not valid");
}

void UTeleportRingsSelector::SetRingActivationPosition(bool Value)
{
	/*
	RelativeLocationToSocket.X = (Value ? 0.0f : 1.0f);
	SetRelativeLocation(RelativeLocationToSocket);
	*/
	SelectorValue = Value;
	if (IsValid(mti_Main)) mti_Main->SetScalarParameterValue(TEXT("EmissionPower"), Value ? 5.0f : 0.5f);

	/*
	if (SelectorValue)
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "SelectorValue = true");
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "SelectorValue = false");
	*/
}
