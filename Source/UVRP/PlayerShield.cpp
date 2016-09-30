// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "PlayerShield.h"


UPlayerShield::UPlayerShield()
{
	bIsOpen = false;
	bIsShieldActive = false;
	PlayAnimation(AnimationClose, false);
	mti_ShieldField = CreateDynamicMaterialInstance(1);
	if (mti_ShieldField != nullptr)
		mti_ShieldField->SetScalarParameterValue("Opacity", 0.0f);
}

bool UPlayerShield::OpenShield()
{
	bool bRet;
	if (!bIsOpen) {
		PlayAnimation(AnimationOpen, false);

		//GetWorldTimerManager().SetTimer(this, &AMatineeActor::CheckPriorityRefresh, 1.0f, true);
		//GetWorld()->GetTimerManager().SetTimer(hWaitForAnimation, FTimerDelegate::CreateUObject(this, &UPlayerShield::TimerFunction), 0.8f, true);
		bRet = true;
	}
	else bRet = false;

	return bRet;
}

bool UPlayerShield::CloseShield()
{
	bool bRet;
	if (bIsOpen) {
		if (mti_ShieldField != nullptr)
			mti_ShieldField->SetScalarParameterValue("Opacity", 0.0f);

		PlayAnimation(AnimationClose, false);
		bRet = true;
	}
	else bRet = false;

	return bRet;
}

void UPlayerShield::StateChanged_Implementation(bool NewState)
{
	//
	return;
}

void UPlayerShield::TimerFunction()
{
	GetWorld()->GetTimerManager().ClearTimer(hWaitForAnimation);

	if (mti_ShieldField != nullptr) {
		if (bIsOpen) {
			mti_ShieldField->SetScalarParameterValue("Opacity", 1.0f);
			bIsShieldActive = true;
		}
	}
	StateChanged(bIsOpen);
}