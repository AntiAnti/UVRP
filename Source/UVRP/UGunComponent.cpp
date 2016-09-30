// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "UGunComponent.h"
#include "UVRPProjectile.h"
//#include "ProjectileBomb.h"
#include "UVRPCharacter.h"
#include "UVRPGameMode.h"
#include "Engine/StaticMeshSocket.h"

// Sets default values for this component's properties
UUGunComponent::UUGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = false;

	// ...
	IsAiming = true;
	AutoFiring = true;
	ShootingSpeedInterval = 0.5f; // в секундах
	ShotTemperatureIncrement = 0.1f;
	ShootingMinimumInterval = 0.3f;
	fTemperature = 0.0f;
	fCumulatedInterval = 0.0f;
	bRestoringTemperature = false;
	CoolingTime = 5.0f; // five sec
	CoolDownTemp = 0.5f;
	UsedByPlayer = false;

	static ConstructorHelpers::FObjectFinder<USoundAttenuation> aeTubeEffect(TEXT("SoundAttenuation'/Game/FirstPerson/Audio/TubeVoiceEffectsBullets.TubeVoiceEffectsBullets'")); //TEXT("SoundAttenuation'/Game/FirstPerson/Audio/TubeVoiceEffects.TubeVoiceEffects'"));
	if (aeTubeEffect.Object != NULL) {
		SoundEffect = (USoundAttenuation*)aeTubeEffect.Object;
	}
	static ConstructorHelpers::FObjectFinder<USoundBase> aeFireSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/Flyby_02.Flyby_02'"));
	if (aeFireSound.Object != NULL) {
		FireSound = (USoundBase*)aeFireSound.Object;
	}
	UpdateMaterialEmission = false;
	ShowBulletTrail = true;
	PrevEmissionParamValue = 0.0f;
}


// Called when the game starts
void UUGunComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	//SetAutoFiring(AutoFiring);
	GetOwner()->GetWorldTimerManager().SetTimer(hCooldownTimer, this, &UUGunComponent::TimerCooldown_Tick, 0.1f, true, 0.1f);
	if (UpdateMaterialEmission) {
		mti_Main = this->CreateDynamicMaterialInstance(0);
	}

	// graphic options setup
	AUVRPGameMode *gm = Cast<AUVRPGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(gm)) {
		ShowBulletTrail = gm->GraphicsOptions.BulletTrail;
	}
}

bool UUGunComponent::Fire()
{
	bool bRet;
	if (ProjectileClass != NULL && !bRestoringTemperature) {
		FVector vSrcPoint = GetGunFiringPoint();

		FRotator rDirection;
		if (AutoFiring)
			rDirection = FindProjectileDirection();
		else
			rDirection = GetSocketRotation(TEXT("FireSource")); // GetComponentRotation();

		AUVRPGameMode* gm = (AUVRPGameMode*)UGameplayStatics::GetGameMode(GetWorld());
		UWorld* const World = GetWorld();
		if (World != NULL) {
			AUVRPProjectile *tmp = World->SpawnActor<AUVRPProjectile>(ProjectileClass, vSrcPoint, rDirection);
			//tmp->ProjectileMovement->MaxSpeed = 350.0f;
			//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, vSrcPoint.ToString());
			if (UsedByPlayer)
				tmp->Tags.Add(TEXT("PLAYER"));
			else
				tmp->Tags.Add(TEXT("AI"));

			if (IsValid(gm) && !gm->GraphicsOptions.BulletTrail) tmp->SetBulletTrailVisibility(false);
		}

		// update info for a soldier AI
		if (UsedByPlayer && IsValid(gm)) {
			gm->sr_LastShotTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		}

		// sound
		if (/*AutoFiring &&*/ FireSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetComponentLocation(), 0.5f, 1.0f, 0.0f, SoundEffect);
		}

		fTemperature += ShotTemperatureIncrement;
		if (fTemperature >= 1.0f) {
			fTemperature = 1.0f;
			bRestoringTemperature = true;
		}
		fCumulatedInterval = 0;
		bRet = true;
	}
	else
		bRet = false;

	return bRet;
}

float UUGunComponent::GetTemperature()
{
	return fTemperature;
}

bool UUGunComponent::IsGunCanShot()
{
	return !bRestoringTemperature;
}

float UUGunComponent::GetWorkingTemperature()
{
	if (bRestoringTemperature)
		return 1.0f;
	else
		return fTemperature;
}

// Called every frame
void UUGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...

	/*
	fCumulatedInterval += DeltaTime;

	if (AutoFiring && IsAiming && fCumulatedInterval >= ShootingSpeedInterval && !bRestoringTemperature) {
		Fire();
	}
	if (bRestoringTemperature) {
		fTemperature -= DeltaTime / CoolingTime;
		if (fTemperature <= 0.0f) {
			fTemperature = 0.0f;
			bRestoringTemperature = false;
		}
	}
	else { // не стреляем - выжидаем интервал между выстрелами
		fTemperature -= CoolDownTemp * DeltaTime / CoolingTime;
		if (fTemperature < 0) fTemperature = 0;
	}

	if (fCumulatedInterval > 1000.0f) fCumulatedInterval = ShootingSpeedInterval;
	*/
}

bool UUGunComponent::TraceToPoint(FVector TargetLocation)
{
	bool bRet = false;
	FVector vSrcPoint = GetGunFiringPoint();
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this->GetAttachmentRootActor());
	FHitResult RV_Hit(ForceInit);
	//vSrcPoint += GetComponentRotation().RotateVector(FVector(10.0f, 0.0f, 0.0f));

	// tracing itself
	GetWorld()->LineTraceSingleByChannel(RV_Hit, vSrcPoint, TargetLocation, ECC_Pawn, RV_TraceParams);

	if (RV_Hit.bBlockingHit)
		if (RV_Hit.GetActor()->IsA(AUVRPCharacter::StaticClass()))
			bRet = true;

	return bRet;
}

void UUGunComponent::SetAutoFiring(bool NewValue)
{
	AutoFiring = NewValue;
	/*
	if (AutoFiring && !hAutoFireTimer.IsValid())
		GetOwner()->GetWorldTimerManager().SetTimer(hAutoFireTimer, this, &UUGunComponent::TimerAutoFire_Tick, ShootingSpeedInterval, true, ShootingSpeedInterval);
	else if (!AutoFiring && hAutoFireTimer.IsValid())
		GetOwner()->GetWorldTimerManager().ClearTimer(hAutoFireTimer);
	*/
}

FVector UUGunComponent::GetGunFiringPoint()
{
	FVector vSrcPoint;

	if (GetSocketByName("FireSource") != nullptr)
		vSrcPoint = GetSocketLocation("FireSource");
	else {
		vSrcPoint = GetComponentLocation();
		vSrcPoint.Z += 20.0f;
	}

	return vSrcPoint;
}

FRotator UUGunComponent::FindProjectileDirection(float Dispersion)
{
	FVector r = FVector(1.0f, 0.0f, 0.0f);
	FVector disp = FVector::ZeroVector;
	FVector vSrcPoint = GetGunFiringPoint();
	ACharacter* chr = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	AUVRPCharacter* ch = nullptr;
	if (IsValid(chr)) ch = Cast<AUVRPCharacter>(chr);

	if (IsValid(ch)) {
		FVector target0 = ch->GetHeadLocation();

		// Decrease dispersion if player is far from soldier
		float dist = (target0 - vSrcPoint).Size();
		if (dist > 1000.0f) Dispersion /= 2.0f;
		disp =  FVector(FMath::FRandRange(-Dispersion, Dispersion),
						FMath::FRandRange(-Dispersion, Dispersion),
						FMath::FRandRange(-Dispersion, Dispersion));

		if (TraceToPoint(target0))
			r = target0 - vSrcPoint;
		else {
			target0 = ch->GetLeftHandLocation();
			if (TraceToPoint(target0))
				r = target0 - vSrcPoint;
			else {
				target0 = ch->GetRightHandLocation();
				if (TraceToPoint(target0))
					r = target0 - vSrcPoint;
				else
					r = ch->GetActorLocation();
			}
		}
	}
	r.Normalize();
	r += disp;

	return r.Rotation();
}

void UUGunComponent::TimerCooldown_Tick()
{
	const float DeltaTime = 0.1f;
	float NewVal;

	fCumulatedInterval += DeltaTime;

	if (AutoFiring && IsAiming && fCumulatedInterval >= ShootingSpeedInterval && !bRestoringTemperature) {
		Fire();
	}
	if (bRestoringTemperature) {
		fTemperature -= DeltaTime / CoolingTime;
		if (fTemperature <= 0.0f) {
			fTemperature = 0.0f;
			bRestoringTemperature = false;
		}
	}
	else { /* не стреляем - выжидаем интервал между выстрелами */
		if (CoolingTime != 0)
			fTemperature -= CoolDownTemp * DeltaTime / CoolingTime;

		if (fTemperature < 0) fTemperature = 0;
	}

	if (fCumulatedInterval > 1000.0f) fCumulatedInterval = ShootingSpeedInterval;

	if (IsValid(mti_Main)) {
		NewVal = (bRestoringTemperature) ? 0.0f : 5.0f * FMath::Pow(1.0f - fTemperature, 2.0f);

		if (NewVal != PrevEmissionParamValue) {
			mti_Main->SetScalarParameterValue(TEXT("Emission Multiplier"), NewVal);
			PrevEmissionParamValue = NewVal;
		}
	}
}