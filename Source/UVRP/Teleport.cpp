// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
//#include "Components/StaticMeshComponent.h"
//#include "Components/SkeletalMeshComponent.h"
#include "GamePlatform.h"
#include "MenuInteractionInterface.h"
#include "TeleportRingsSelector.h"
#include "UVRPGameMode.h"
#include "Engine.h"
#include "Teleport.h"


// Sets default values
ATeleport::ATeleport()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	TeleportUnlockInterval = 15.0f;
	ShowFieldEffects = true;
	SelectorState = false;
}

// Called when the game starts or when spawned
void ATeleport::BeginPlay()
{
	Super::BeginPlay();

	// difficulty and graphics setup
	AUVRPGameMode *gm = Cast<AUVRPGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(gm)) {
		TeleportUnlockInterval = gm->TeleportDecryptTimeout;
		ShowFieldEffects = gm->GraphicsOptions.PortalRefraction;
	}
}

// Called every frame
void ATeleport::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

bool ATeleport::OnClick(FVector HitPoint)
{
	bool bRet = false;

	if (IsLockOpen) {
		//if (IsValid(RingSelector)) {
		bRet = SelectorState; // (RingSelector->IsRingCanBeActivated());

		if (bRet) {
			SetRingsStatusVar(RingsStatus + 1);
			RingSelector->SetRingActivationPosition(false);
			SelectorState = false;
		}
		else {
			SetLockOpenVar(false);
			SetRingsStatusVar(0);
		}
		//}

		OnLockClick(bRet);
	}

	return bRet;
}

bool ATeleport::IsUsingCustomSound()
{
	// don't use default beep sound when clicking teleport lock
	return true;
}

USkeletalMeshComponent* ATeleport::GetTeleportMesh_Implementation()
{
	return nullptr;
}

