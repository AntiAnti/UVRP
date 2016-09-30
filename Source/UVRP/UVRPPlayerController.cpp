// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "IHeadMountedDisplay.h"

#include "UVRPPlayerController.h"

void AUVRPPlayerController::UpdateRotation(float DeltaTime)
{
	// Calculate Delta to be applied on ViewRotation
	FRotator DeltaRot(RotationInput);

	FRotator NewControlRotation = GetControlRotation();

	if (PlayerCameraManager) {
		PlayerCameraManager->ProcessViewRotation(DeltaTime, NewControlRotation, DeltaRot);
	}

	SetControlRotation(NewControlRotation);

	if (!PlayerCameraManager /*|| !PlayerCameraManager->bFollowHmdOrientation*/) {
		if (GEngine->HMDDevice.IsValid() && GEngine->HMDDevice->IsHeadTrackingAllowed()) {
			FQuat HMDOrientation;
			FVector HMDPosition;

			// Disable bUpdateOnRT if using this method.
			GEngine->HMDDevice->GetCurrentOrientationAndPosition(HMDOrientation, HMDPosition);

			FRotator NewViewRotation = HMDOrientation.Rotator();

			// Only keep the yaw component from the controller.
			NewViewRotation.Yaw += NewControlRotation.Yaw;

			SetViewRotation(NewViewRotation);
		}
	}

	APawn* const P = GetPawnOrSpectator();
	if (P) {
		P->FaceRotation(NewControlRotation, DeltaTime);
	}
}

void AUVRPPlayerController::SetControlRotation(const FRotator& NewRotation)
{
	ControlRotation = NewRotation;

	// Anything that is overriding view rotation will need to 
	// call SetViewRotation() after SetControlRotation().
	SetViewRotation(NewRotation);

	if (RootComponent && RootComponent->bAbsoluteRotation) {
		RootComponent->SetWorldRotation(GetControlRotation());
	}
}

void AUVRPPlayerController::SetViewRotation(const FRotator& NewRotation)
{
	ViewRotation = NewRotation;
}

FRotator AUVRPPlayerController::GetViewRotation() const
{
	return ViewRotation;
}