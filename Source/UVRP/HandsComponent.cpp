// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "HandsComponent.h"
#include "DrawDebugHelpers.h"
#include "HeadMountedDisplay.h"
#include "Engine.h"

//#define HISTORY_INDEX(u)		((HistoryId - u) + 180) % 180

UHandsComponent::UHandsComponent()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smHandsLong(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/Hands/sm_PlayerHands.sm_PlayerHands'"));
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> smHandsShort(TEXT("SkeletalMesh'/Game/FirstPerson/Objects/Hands/sm_PlayerHandsShort.sm_PlayerHandsShort'"));

	if (smHandsLong.Object != NULL) {
		this->SetSkeletalMesh((USkeletalMesh*)smHandsLong.Object);
		MeshHandsLong = (USkeletalMesh*)smHandsLong.Object;
	}
	if (smHandsShort.Object != NULL) {
		MeshHandsShort = (USkeletalMesh*)smHandsShort.Object;
	}

	if (IsValid(this->SkeletalMesh)) {
		if (GetSocketByName("Socket_CollarboneLeft") != nullptr)
			LeftCollarboneLocation = GetSocketLocation("Socket_CollarboneLeft");
		else {
			LeftCollarboneLocation = GetComponentLocation();
		}
		if (GetSocketByName("Socket_CollarboneRight") != nullptr)
			RightCollarboneLocation = GetSocketLocation("Socket_CollarboneRight");
		else {
			RightCollarboneLocation = GetComponentLocation();
		}
	}
	DefaultHandScale = 1.0f;
	HandScale = 1.0f;
	DefaultHandScale = 1.0f;
	CollarboneMaxOffset = 20.0f; 
	LenUpperarm = 0.0f;
	LenForearm = 0.0f;
	bShowHands = true;

	HistoryId = -1;
}

void UHandsComponent::BeginPlay()
{
	//DebugFile.Open(L"c:\\saverotationdata.dat", CFile::modeCreate | CFile::modeWrite, NULL);
	Super::BeginPlay();
}

void UHandsComponent::UpdateHandsMeshType(bool ShowHands)
{
	if (bShowHands != ShowHands) {
		bShowHands = ShowHands;
		if (bShowHands)
			SetSkeletalMesh(MeshHandsLong);
		else
			SetSkeletalMesh(MeshHandsShort);
	}
}

void UHandsComponent::UpdateHandsMesh()
{
	FVector v1, v2, v3;
	float lenUpper, lenFore, direct;

	if (GetSocketByName("Socket_CollarboneLeft") != nullptr)
		LeftCollarboneLocation = GetSocketLocation("Socket_CollarboneLeft");
	else {
		LeftCollarboneLocation = GetComponentLocation();
	}
	if (GetSocketByName("Socket_CollarboneRight") != nullptr)
		RightCollarboneLocation = GetSocketLocation("Socket_CollarboneRight");
	else {
		RightCollarboneLocation = GetComponentLocation();
	}

	if (LenUpperarm == 0)
		LenUpperarm = (GetBoneLocation("PlayerHands_LUpperarm") - GetBoneLocation("PlayerHands_LForearm")).Size();
	if (LenForearm == 0)
		LenForearm = (GetBoneLocation("PlayerHands_LForearm") - GetBoneLocation("PlayerHands_LHand")).Size();

	FName _CollarboneName = "PlayerHands_RCollarbone";
	FName _CollarboneSocketName = "Socket_CollarboneRight";
	FName _UpperarmNameR = "PlayerHands_RUpperarm";
	FName _UpperarmNameL = "PlayerHands_LUpperarm";
	FName _ForearmName = "PlayerHands_RForearm";
	FName _HandName = "PlayerHands_RHand";
	FVector CollarboneLocation = RightCollarboneLocation;
	FVector HandLocation = RightHandLocation;
	FRotator ToolRotation = RightToolRotation;
	// ------------------------------- ДЛИНА ------------------------------- //

	// Длины костей
	direct = (GetBoneLocation(_UpperarmNameR) - HandLocation).Size();

	HandScale = direct / (LenUpperarm + LenForearm);
	if (HandScale < DefaultHandScale) {
		HandScale = DefaultHandScale;
	} else if (HandScale > DefaultHandScale) {
		if (HandScale - 0.1f > DefaultHandScale)
			DefaultHandScale = HandScale - 0.1f; // исправляем существовавшую, видимо, погрешность
	}
	lenUpper = LenUpperarm * HandScale;
	lenFore = LenForearm * HandScale;

	// right hand
	UpdateHandRotators(RightUpperarmRotation, RightForearmRotation, "PlayerHands_RCollarbone", "PlayerHands_RUpperarm",
		"PlayerHands_RForearm", "PlayerHands_RHand", RightHandLocation, RightToolRotation, lenUpper, lenFore, direct, true);

	// left hand
	direct = (GetBoneLocation(_UpperarmNameL) - LeftHandLocation).Size();
	UpdateHandRotators(LeftUpperarmRotation, LeftForearmRotation, "PlayerHands_LCollarbone", "PlayerHands_LUpperarm",
		"PlayerHands_LForearm", "PlayerHands_LHand", LeftHandLocation, LeftToolRotation, lenUpper, lenFore, direct, false);
	
	// сохранение истории ориентации
	SaveCurrentFrameRotation();
	
	return;
}

void UHandsComponent::CalibrateHandLength(bool RightHand)
{
	DefaultHandScale = 0.1f;
	/*
	float direct;

	FName _UpperarmNameR = "PlayerHands_RUpperarm";
	FName _UpperarmNameL = "PlayerHands_LUpperarm";

	if (RightHand)
		direct = (GetBoneLocation(_UpperarmNameR) - RightHandLocation).Size();
	else
		direct = (GetBoneLocation(_UpperarmNameR) - LeftHandLocation).Size();

	DefaultHandScale = direct / (LenUpperarm + LenForearm);
	*/
}

void UHandsComponent::UpdateHandRotators(FRotator &retUpperarmRotation, FRotator &retForearmRotation, FName _CollarboneName, FName _UpperarmName, FName _ForearmName, FName _HandName, FVector HandLocation, FRotator ToolRotation, float lenUpper, float lenFore, float direct, bool hand)
{
	float angle1, angle2;
	float koef = (hand ? 1.0f : -1.0f);
	FRotator rMainPlaneRotation;

	// ------------------------------ КЛЮЧИЦА ------------------------------ //
	// Коэффициент поворота ключицы от 0 до 1 в степени 1/2
	/*
	float CollarboneOffsetKoef = FMath::Sqrt((direct >= (lenUpper + lenFore)) ? 1 : direct / (lenUpper + lenFore));

	FTransform trCBone;
	trCBone.SetRotation((GetBoneLocation("PlayerHands_RHand") - GetBoneLocation("PlayerHands_RCollarbone")).Rotation().Quaternion());
	trCBone.SetTranslation(GetBoneLocation("PlayerHands_RCollarbone"));

	FTransform trCBoneAdd;
	trCBoneAdd.SetRotation(FRotator(0.0f, CollarboneMaxOffset*CollarboneOffsetKoef, 0.0f).Quaternion());
	trCBoneAdd.SetTranslation(trCBone.GetTranslation());

	// Получаем мухахатрицу для ключицы
	trCBone = trCBoneAdd * trCBone;
	RightCollarboneRotation = trCBone.Rotator();
	*/

	// ------------------------------- ОБЩЕЕ ------------------------------- //
	// Углы между прямой и плечом, между плечом и предплечьем
	angle1 = FMath::RadiansToDegrees(FMath::Acos((lenUpper*lenUpper + direct*direct - lenFore*lenFore) / (2 * lenUpper * direct)));
	angle2 = FMath::RadiansToDegrees(FMath::Acos((lenUpper*lenUpper + lenFore*lenFore - direct*direct) / (2 * lenUpper * lenFore)));

	// Нужно посчитать наклон плоскости вращения локтя
	rMainPlaneRotation = (HandLocation - GetBoneLocation(_UpperarmName)).Rotation(); // плоскость, в которой находится рука

	const float fPitchMultiplier = 0.7f;	// коэффициенты участия
	float fRollMultiplier;					// коэффициенты участия
	if ((ToolRotation.Roll * koef) < 0.0f)	// когда локоть идет вверх, он поднимается слабо
		fRollMultiplier = 0.15f;
	else
		fRollMultiplier = 0.7f;

	float newRoll = (ToolRotation.Roll)  * fRollMultiplier;
	rMainPlaneRotation.Roll = FMath::Pow(newRoll / (90.0f * fRollMultiplier), 2.0f) * (90.0f * fRollMultiplier);
	if (ToolRotation.Roll < 0) rMainPlaneRotation.Roll = -rMainPlaneRotation.Roll;

	rMainPlaneRotation.Roll += FMath::ClampAngle(ToolRotation.Pitch, -90.0f, 90.0f) * fPitchMultiplier * koef;
	// clamping!
	rMainPlaneRotation.Roll = FMath::ClampAngle(rMainPlaneRotation.Roll, -90.0f, 90.0f);

	// ------------------------------- ПЛЕЧО ------------------------------- //
	FTransform plane, rotUpper, rotFore;

	plane.SetRotation(rMainPlaneRotation.Quaternion());
	plane.SetTranslation(GetBoneLocation(_UpperarmName));
	rotUpper.SetRotation(FRotator(0.0f, angle1 * koef, 0.0f).Quaternion());
	rotUpper.SetTranslation(plane.GetLocation());

	plane = rotUpper * plane;
	retUpperarmRotation = plane.Rotator(); // <--------- return

	// ---------------------------- ПРЕДПЛЕЧЬЕ ----------------------------- //
	plane.SetRotation(rMainPlaneRotation.Quaternion());
	plane.SetTranslation(GetBoneLocation(_ForearmName));
	rotFore.SetRotation(FRotator(0.0f, koef * (-180.0f + angle1 + angle2), 0.0f).Quaternion());
	rotFore.SetTranslation(plane.GetLocation());

	plane = rotFore * plane;
	retForearmRotation = plane.Rotator(); // <--------- return
}

FVector UHandsComponent::GetFrontVector()
{
	return GetComponentRotation().Vector();
}

// ret.x * X + ret.y * Y + ret.Z * Z + ret.W = 0
FVector4 UHandsComponent::GetPlaneByThreePoints(FVector p1, FVector p2, FVector p3)
{
	FVector4 ret;
	FVector v1, v2, v3;
	/*
	float A, B, C, D, a1, b1;
	float mat[3][3];
	mat[0][0] = p2.X - p1.X;	mat[1][0] = p2.Y - p1.Y;	mat[2][0] = p2.Z - p1.Z;
	mat[0][1] = p3.X - p1.X;	mat[1][1] = p3.Y - p1.Y;	mat[2][1] = p3.Z - p1.Z;
	mat[0][2] = -p1.X;			mat[1][2] = -p1.Y;			mat[2][2] = -p1.Z;
	*/

	v1 = p2 - p1;
	v2 = p3 - p1;
	v3 = v1 * v2; // нормаль

	ret.X = v3.X;
	ret.Y = v3.Y;
	ret.Z = v3.Z;
	ret.W = -v3.X * p1.X - v3.Y * v1.Y - v3.Z * v1.Z;

	return ret;
}

void UHandsComponent::SaveCurrentFrameRotation()
{
	FRotator rot;
	FVector pos;
	HistoryId++;
	if (HistoryId >= 180) HistoryId = 0;

	UHeadMountedDisplayFunctionLibrary::GetOrientationAndPosition(rot, pos);
	RotationData_HMD[HistoryId].X = rot.Yaw;
	RotationData_HMD[HistoryId].Y = rot.Pitch;
	RotationData_HMD[HistoryId].Z = rot.Roll;

	RotationData_Left[HistoryId].X = LeftToolRotation.Yaw;
	RotationData_Left[HistoryId].Y = LeftToolRotation.Pitch;
	RotationData_Left[HistoryId].Z = LeftToolRotation.Roll;

	RotationData_Right[HistoryId].X = RightToolRotation.Yaw;
	RotationData_Right[HistoryId].Y = RightToolRotation.Pitch;
	RotationData_Right[HistoryId].Z = RightToolRotation.Roll;

	MovingData_Left[HistoryId] = LeftHandLocation;
	MovingData_Right[HistoryId] = RightHandLocation;

	RotationData_Time[HistoryId] = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	/*
	DebugFile.Write(&RotationData_HMD[HistoryId].X, sizeof(float));
	DebugFile.Write(&RotationData_Left[HistoryId].X, sizeof(float));
	DebugFile.Write(&RotationData_Left[HistoryId].Y, sizeof(float));
	DebugFile.Write(&RotationData_Left[HistoryId].Z, sizeof(float));
	DebugFile.Write(&RotationData_Right[HistoryId].X, sizeof(float));
	DebugFile.Write(&RotationData_Right[HistoryId].Y, sizeof(float));
	DebugFile.Write(&RotationData_Right[HistoryId].Z, sizeof(float));
	*/
}

bool UHandsComponent::IsObjectRotating()
{
	int32 n, m;
	float RotSpeedHmd = 0.0f, RotSpeedLeft = 0.0f, RotSpeedRight = 0.0f, MovSpeedLeft = 0.0f, MovSpeedRight = 0.0f;
	for (int32 i = 0; i < 5; i++) {
		n = ((HistoryId - i) + 90) % 90;
		m = ((HistoryId - i-5) + 90) % 90;

		RotSpeedHmd += (RotationData_HMD[n].X - RotationData_HMD[m].X) / (RotationData_Time[n] - RotationData_Time[m]);
		RotSpeedLeft += (RotationData_Left[n].X - RotationData_Left[m].X) / (RotationData_Time[n] - RotationData_Time[m]);
		RotSpeedRight += (RotationData_Right[n].X - RotationData_Right[m].X) / (RotationData_Time[n] - RotationData_Time[m]);
		MovSpeedLeft += (FVector(MovingData_Left[n] - MovingData_Left[m]).Size()) / (RotationData_Time[n] - RotationData_Time[m]);
		MovSpeedRight += (FVector(MovingData_Right[n] - MovingData_Right[m]).Size()) / (RotationData_Time[n] - RotationData_Time[m]);
	}
	RotSpeedHmd /= 5.0f;
	RotSpeedLeft /= 5.0f;
	RotSpeedRight /= 5.0f;
	MovSpeedLeft /= 5.0f;
	MovSpeedRight /= 5.0f;

	if (RotSpeedHmd > 20.0f /* degrees per second */) {
		//if (RotSpeedLeft >= 0.85f * RotSpeedHmd && RotSpeedRight >= 0.85f * RotSpeedHmd) {
			if (MovSpeedLeft >= 10.0f || MovSpeedRight >= 10.0f) {
				return true;
			}
		//}
	}
	
	//GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, FString::SanitizeFloat(MovSpeedRight));
	return false;
}