// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "GamePlatform.h"
#include "UVRPCharacter.h"
#include "AutomaticTurret.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"

AAutomaticTurret::AAutomaticTurret()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smTurretAsset(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/TurretPlatform/sm_Turret.sm_Turret'"));
	if (smTurretAsset.Object != NULL)
		GetSkeletalMeshComponent()->SetSkeletalMesh(smTurretAsset.Object);
	else
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Error. Asset not found: sm_Turret.sm_Turret");

	LaserBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserBeam"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> peLaserBeam(TEXT("ParticleSystem'/Game/FirstPerson/Effects/ps_LaserBeam.ps_LaserBeam'"));
	LaserBeam->SetupAttachment(GetSkeletalMeshComponent(), TEXT("FireSocket"));
	LaserBeam->SetRelativeLocation(FVector::ZeroVector);
	if (peLaserBeam.Object != NULL) LaserBeam->SetTemplate(peLaserBeam.Object);
	LaserBeam->bAutoActivate = false;
	LaserBeam->SetVisibility(false);
	//LaserBeam->DeactivateSystem();

	LaserTargetingBeam = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserTargetingBeam"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> peLaserTargetingBeam(TEXT("ParticleSystem'/Game/FirstPerson/Effects/ps_LaserBeamWhite.ps_LaserBeamWhite'"));
	LaserTargetingBeam->SetupAttachment(GetSkeletalMeshComponent(), TEXT("FireSocket"));
	LaserTargetingBeam->SetRelativeLocation(FVector::ZeroVector);
	if (peLaserTargetingBeam.Object != NULL) LaserTargetingBeam->SetTemplate(peLaserTargetingBeam.Object);
	LaserTargetingBeam->bAutoActivate = false;
	LaserTargetingBeam->SetVisibility(false);
	//LaserTargetingBeam->DeactivateSystem();
	//LaserTargetingBeam->ActivateSystem(true);

	KillingCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("KillingCollider"));
	KillingCollider->SetupAttachment(GetSkeletalMeshComponent(), TEXT("KillingColliderSocket"));
	KillingCollider->SetRelativeLocation(FVector::ZeroVector);
	KillingCollider->SetRelativeRotation(FRotator::ZeroRotator);
	KillingCollider->SetBoxExtent(FVector(1.2f, 1.2f, 2.0f));
	KillingCollider->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	KillingCollider->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	KillingCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	KillingCollider->SetCollisionResponseToChannel(EEC_ProjectileChannel, ECollisionResponse::ECR_Block);
	KillingCollider->OnComponentHit.AddDynamic(this, &AAutomaticTurret::OnPlayerHit);

	static ConstructorHelpers::FObjectFinder<USoundBase> sbFireSound(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/pack/turret_1.turret_1'"));
	if (sbFireSound.Object != NULL) FireSound = sbFireSound.Object;

	TargetingHorizontalSpeed = 1.8f;
	TargetingVerticalSpeed = 1.8f;
	ShootingDamagePower = 200.0f;
	CurrentRotationHorizontal = 0.0f;
	CurrentRotationVertical = 0.0f;
	bIsActive = false;
	CurrentShootingTarget = FVector::ZeroVector;
	ChargingInterval = 1.5f;
	ShootingDischargeInterval = 0.5f;
	GunHeatingInterval = 0.8f;
	nCurrentState = _TURRET_STATE_DISABLED;
	Health = 1.0f;
	HitDamage = 0.25f;

	FQuat r1 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretRotator", EBoneSpaces::WorldSpace);
	FQuat r2 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretGun", EBoneSpaces::WorldSpace);
	CurrentRotationHorizontal = r1.Rotator().Yaw;
	CurrentRotationVertical = r2.Rotator().Pitch;

	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void AAutomaticTurret::BeginPlay()
{
	Super::BeginPlay();
	// initialize bones
	CurrentEventLength = 0.0f;
	FQuat r1 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretRotator", EBoneSpaces::WorldSpace);
	FQuat r2 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretGun", EBoneSpaces::WorldSpace);
	CurrentRotationHorizontal = r1.Rotator().Yaw;
	CurrentRotationVertical = r2.Rotator().Pitch;
}

void AAutomaticTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentEventLength += DeltaTime;

	if (bIsActive) {
		const float TargetingDeviation = 0.01f;
		float f;
		FVector vSource = GetSkeletalMeshComponent()->GetSocketLocation("FindTargetSocket");
		//FVector vFireDest = vFireSocket + GetSkeletalMeshComponent()->GetSocketQuaternion("").
		FTransform tFire = GetSkeletalMeshComponent()->GetSocketTransform("FireSocket");
		FVector vFireSource = tFire.GetLocation();
		FVector vFireDest = CurrentShootingTarget; // vFireSource + tFire.GetRotation().RotateVector(FVector::ForwardVector);

		FVector v2 = FVector::ZeroVector;
		FRotator test;
		FVector a;
		FRotator b;

		switch (nCurrentState) {
		case _TURRET_STATE_AIMING_PAWN:
		case _TURRET_STATE_AIMING_CHARGER:
			// update target if necessary
			if (!CheckCurrentShootingTarget()) {
				if (nCurrentState == _TURRET_STATE_AIMING_PAWN) {
					CurrentShootingTarget = GetShootingTarget();

					FVector v1 = CurrentShootingTarget - vSource;
					v1.Z = 0.0f;
					TargetRotationHorizontal = v1.Rotation().Yaw;
					if (TargetRotationHorizontal < 0.0f) {
						TargetRotationHorizontal += 360.0f;
						if (CurrentRotationHorizontal < 180.0f)
							CurrentRotationHorizontal += 360.0f;
					}
					v2 = GetSkeletalMeshComponent()->GetSocketLocation("CalcHorRotSocket");
					v2.X += FMath::Cos(FMath::DegreesToRadians(TargetRotationHorizontal)) * 6.9f;
					v2.Y += FMath::Sin(FMath::DegreesToRadians(TargetRotationHorizontal)) * 6.9f;
					v2.Z += 34.846f;

					v1 = CurrentShootingTarget - vSource;
					TargetRotationVertical = v1.Rotation().Pitch;
				}
				else {
					GetSkeletalMeshComponent()->GetSocketWorldLocationAndRotation("ChargingDirectionSocket", a, b);
					v2 = a - GetSkeletalMeshComponent()->GetBoneLocation("b_TurretBase", EBoneSpaces::WorldSpace);
					f = v2.Rotation().Yaw;
					TargetRotationHorizontal = 0.f;
					TargetRotationVertical = 45.0f;
				}
			}
			test.Yaw = CurrentRotationHorizontal;
			test.Pitch = -10.0f;

			float inc;
			inc = (TargetRotationHorizontal - CurrentRotationHorizontal) * TargetingHorizontalSpeed * DeltaTime;
			CurrentRotationHorizontal += inc;
			if (FMath::Abs(CurrentRotationHorizontal - TargetRotationHorizontal) <= inc) CurrentRotationHorizontal = TargetRotationHorizontal;

			inc = (TargetRotationVertical - CurrentRotationVertical) * TargetingVerticalSpeed * DeltaTime;
			CurrentRotationVertical += inc;
			if (FMath::Abs(CurrentRotationVertical - TargetRotationVertical) <= inc) CurrentRotationVertical = TargetRotationVertical;

			/*
			CurrentRotationHorizontal = FMath::FInterpTo(CurrentRotationHorizontal, TargetRotationHorizontal, DeltaTime, TargetingHorizontalSpeed);
			CurrentRotationVertical = FMath::FInterpTo(CurrentRotationVertical, TargetRotationVertical, DeltaTime, TargetingVerticalSpeed);
			*/

			// aiming finished
			if (FMath::Abs(CurrentRotationHorizontal - TargetRotationHorizontal) < TargetingDeviation &&
				FMath::Abs(CurrentRotationVertical - TargetRotationVertical) < TargetingDeviation) {
				if (nCurrentState == _TURRET_STATE_AIMING_PAWN) {
					nCurrentState = _TURRET_STATE_PREPAREGUN;
					CurrentEventLength = 0.0f;
					AimingFinished();
					ShootingPreparingStarted();
				}
				else if (nCurrentState == _TURRET_STATE_AIMING_CHARGER) {
					nCurrentState = _TURRET_STATE_CHARGING;
					CurrentEventLength = 0.0f;

					AimingFinished();
				}
			}
			
			break;
		case _TURRET_STATE_PREPAREGUN:
			if (CurrentEventLength >= GunHeatingInterval) {
				ShootingPreparingFinished();
				nCurrentState = _TURRET_STATE_SHOOTING;
				CurrentEventLength = 0.0f;
				ShootingBeamStarted();
			} /*else 
				DrawDebugLine(GetWorld(), vFireSource, vFireDest, FColor::White, false, 0.1f, 0, 1.0f);*/

			break;
		case _TURRET_STATE_SHOOTING:
			if (CurrentEventLength >= ShootingDischargeInterval) {
				ShootingBeamFinished();
				nCurrentState = _TURRET_STATE_AIMING_CHARGER;
				CurrentEventLength = 0.0f;
				CurrentShootingTarget = FVector::ZeroVector;
				AimingStarted();
			}
			else {
				if (IsValid(TurretTarget)) {
					CheckTargetForDamage(DeltaTime);
				}
			}

			break;
		case _TURRET_STATE_CHARGING:
			if (CurrentEventLength >= ChargingInterval) {
				ShootingBeamFinished();
				nCurrentState = _TURRET_STATE_AIMING_PAWN;
				CurrentEventLength = 0.0f;
				AimingStarted();
			}
			break;
		}
	}

	if (Health <= 0.0f) {
		bIsActive = false;
		SetActorTickEnabled(false);
	}
}

void AAutomaticTurret::Activate()
{
	if (bIsActive) return;

	FQuat r1 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretRotator", EBoneSpaces::WorldSpace);
	FQuat r2 = GetSkeletalMeshComponent()->GetBoneQuaternion("b_TurretGun", EBoneSpaces::WorldSpace);
	CurrentRotationHorizontal = r1.Rotator().Yaw;
	CurrentRotationVertical = r2.Rotator().Pitch;
	if (CurrentRotationHorizontal < 0.0f) CurrentRotationHorizontal += 360.0f;

	APawn *player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (player != nullptr)
		TurretTarget = Cast<AUVRPCharacter>(player);
	else
		TurretTarget = NULL;

	nCurrentState = _TURRET_STATE_AIMING_PAWN;
	SetActorTickEnabled(true);
	bIsActive = true;
}

FVector AAutomaticTurret::GetShootingTarget()
{
	//FVector vSource, v2;
	//FRotator rDirection;

	return TurretTarget->GetHeadLocation();

	/*
	float z1, z2;
	FVector hor[5];
	if (TurretTarget) {
		hor[0] = TurretTarget->GetLeftHandLocation();
		hor[2] = TurretTarget->GetHeadLocation();
		hor[4] = TurretTarget->GetRightHandLocation();
		hor[1] = (hor[2] + hor[0]) / 2.0f;
		hor[3] = (hor[2] + hor[4]) / 2.0f;
		z1 = TurretTarget->GetFootLocation().Z;
		z2 = hor[2].Z;

		vSource = GetSkeletalMeshComponent()->GetSocketLocation("FindTargetSocket");
		if (TraceToPoint(vSource, hor[2])) {
			return hor[2];
		}
		else if (TraceToPoint(vSource, hor[0])) {
			return hor[0];
		}
		else if (TraceToPoint(vSource, hor[4])) {
			return hor[4];
		}
		else {
			for (int z = z2; z >= z1; z -= (z2 - z1) / 5.0f) {
				for (int h = 0; h < 5; h++) {
					v2 = FVector(hor[h].X, hor[h].Y, z);
					if (TraceToPoint(vSource, v2))
						return v2;
				}
			}
		}
	}
	else
		return FVector::ZeroVector;

	return hor[2];
	*/
}

bool AAutomaticTurret::TraceToPoint(FVector SourceLocation, FVector TargetLocation)
{
	bool bRet = false;
	FVector vSrcPoint = SourceLocation;
	FCollisionQueryParams RV_TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
	FHitResult RV_Hit(ForceInit);

	GetWorld()->LineTraceSingleByChannel(RV_Hit, vSrcPoint, TargetLocation, ECC_Pawn, RV_TraceParams);

	if (RV_Hit.bBlockingHit && RV_Hit.GetActor() != nullptr)
		if (RV_Hit.GetActor()->IsA(AUVRPCharacter::StaticClass()))
			bRet = true;

	return bRet;
}

bool AAutomaticTurret::CheckCurrentShootingTarget()
{
	if (nCurrentState == _TURRET_STATE_AIMING_PAWN)
		return TraceToPoint(GetSkeletalMeshComponent()->GetSocketLocation("FindTargetSocket"), CurrentShootingTarget);
	else {
		FVector a;
		FRotator b;
		GetSkeletalMeshComponent()->GetSocketWorldLocationAndRotation("ChargingDirectionSocket", a, b);
		GetSkeletalMeshComponent()->GetSocketWorldLocationAndRotation("ChargingDirectionSocket", a, b);
		FVector v2 = a - GetSkeletalMeshComponent()->GetBoneLocation("b_TurretBase", EBoneSpaces::WorldSpace);

		return (TargetRotationHorizontal == v2.Rotation().Yaw && TargetRotationVertical == 55.0f);
	}
}

void AAutomaticTurret::CheckTargetForDamage(float DeltaTime)
{
	FTransform tr = GetSkeletalMeshComponent()->GetSocketTransform(TEXT("FindTargetSocket"));
	if (TraceToPoint(tr.GetLocation(), tr.GetLocation() + tr.GetRotation().GetForwardVector() * 1000.0f)) {
		TurretTarget->Damage(1.0f * DeltaTime);
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::White, "Not Targeting Player");
	}
}

void AAutomaticTurret::OnPlayerHit(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Health -= HitDamage;

	if (Health <= 0.0f) {
		Health = 0.0f;

		LaserBeam->DestroyComponent();
		KillingCollider->DestroyComponent();
		SetActorTickEnabled(false);
		bIsActive = false;

		OnTurretDestroyed();
	}
}

void AAutomaticTurret::AimingFinished_Implementation()
{
}

void AAutomaticTurret::AimingStarted_Implementation()
{
}

void AAutomaticTurret::ShootingPreparingStarted_Implementation()
{
	LaserTargetingBeam->Activate();
	LaserTargetingBeam->SetActive(true);
	LaserTargetingBeam->ActivateSystem();
	LaserTargetingBeam->SetVisibility(true);
}

void AAutomaticTurret::ShootingPreparingFinished_Implementation()
{
	LaserTargetingBeam->DeactivateSystem();
	LaserTargetingBeam->Deactivate();
	LaserTargetingBeam->SetActive(false);
	LaserTargetingBeam->SetVisibility(false);
}

void AAutomaticTurret::ShootingBeamStarted_Implementation()
{
	//if (!LaserBeam->IsVisible()) {
		//DrawDebugLine(GetWorld(), vFireSource, vFireDest, FColor::Red, false, 0.1f, 0, 5.0f);
		LaserBeam->Activate();
		LaserBeam->SetActive(true);
		LaserBeam->ActivateSystem();
		LaserBeam->SetVisibility(true);
	//}
}

void AAutomaticTurret::ShootingBeamFinished_Implementation()
{
	//if (LaserBeam->IsVisible()) {
		LaserBeam->DeactivateSystem();
		LaserBeam->Deactivate();
		LaserBeam->SetActive(false);
		LaserBeam->SetVisibility(false);

		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Exit");
	//}
}

void AAutomaticTurret::OnTurretDestroyed_Implementation()
{
	GetSkeletalMeshComponent()->DestroyComponent();
}
