// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "HandMenuList.h"
#include "UVRPGameMode.h"
#include "HandMenuSlider.h"

AHandMenuSlider::AHandMenuSlider()
{
	SliderMesh = CreateDefaultSubobject<UStaticMeshComponent>("SliderMenuMesh");
	SliderMesh->SetupAttachment(GetRootComponent());
	SliderMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	SliderMesh->SetVisibility(true);
	SliderMesh->SetCollisionObjectType(ECC_ControllerResponses);
	SliderMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SliderMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAssetPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/menu_slider.menu_slider'"));
	if (smAssetPlane.Object != NULL) {
		SliderMesh->SetStaticMesh(smAssetPlane.Object);

		int count = SliderMesh->GetMaterials().Num();
		if (count > 0)
			mtiFrame = SliderMesh->CreateDynamicMaterialInstance(0);
		if (count > 1)
			mtiSlider = SliderMesh->CreateDynamicMaterialInstance(1);
	}
	vBaseColor = FLinearColor(0.00857f, 0.238f, 0.371f, 1.0f);
	vActiveColor = FLinearColor(0.3f, 1.1f, 0.2f, 1.0f);

	fProgressBarValue = 0.0f;

	SliderCollider = CreateDefaultSubobject<UBoxComponent>("SliderCollider");
	SliderCollider->SetupAttachment(GetRootComponent());
	SliderCollider->SetRelativeLocation(FVector(14.3f, -0.05f, 11.75f));
	SliderCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SliderCollider->SetCollisionObjectType(ECC_ControllerResponses);
	SliderCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // :ECR_Ignore);
	SliderCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); // :ECR_Ignore);
	SliderCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	SliderCollider->SetBoxExtent(FVector(9.75f, 0.1f, 1.25f));
	SliderCollider->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuSlider::Slider_OnBeginOverlap);
	SliderCollider->OnComponentEndOverlap.AddDynamic(this, &AHandMenuSlider::Slider_OnEndOverlap);

	ButtonCollider = CreateDefaultSubobject<UBoxComponent>("ButtonCollider");
	ButtonCollider->SetupAttachment(GetRootComponent());
	ButtonCollider->SetRelativeLocation(FVector(14.3f, -0.05f, 6.4f));
	ButtonCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ButtonCollider->SetCollisionObjectType(ECC_ControllerResponses);
	ButtonCollider->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap); // :ECR_Ignore);
	ButtonCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block); // :ECR_Ignore);
	ButtonCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ButtonCollider->SetBoxExtent(FVector(12.0f, 0.1f, 2.4f));
	ButtonCollider->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuSlider::Button_OnBeginOverlap);
	ButtonCollider->OnComponentEndOverlap.AddDynamic(this, &AHandMenuSlider::Button_OnEndOverlap);

	ActiveButton = 0;
	MaxIntVal = 1;

	static ConstructorHelpers::FObjectFinder<USoundBase> swTestVolume(TEXT("SoundWave'/Game/FirstPerson/Audio/sound/Engine_Bass_001.Engine_Bass_001'"));
	if (swTestVolume.Object != NULL) {
		TestVolumeSound = (USoundBase*)swTestVolume.Object;
	}
}

void AHandMenuSlider::SetType(EMenuSetupName ItemType)
{
	MenuParam = ItemType;

	float tu = 0.0f, tv = 0.0f;
	AUVRPGameMode *gm = Cast<AUVRPGameMode>(GetWorld()->GetAuthGameMode());
	if (IsValid(gm)) {
		switch (MenuParam) {
			case EMenuSetupName::MS_MusicVolume:
				SetValue(gm->GO_MusicVolume);
				tu = 0.5f; tv = 0.0f;
				break;
			case EMenuSetupName::MS_SoundVolume:
				SetValue(gm->GO_SoundVolume);
				tu = 0.0f; tv = 0.0f;
				break;
			case EMenuSetupName::MS_VoiceVolume:
				SetValue(gm->GO_VoiceVolume);
				tu = 0.5f; tv = 0.3333f;
				break;
			case EMenuSetupName::MS_DiffLevel:
				SetMaxNumericValue(gm->GO_Max_DifficultyLevel);
				tu = 0.0f; tv = 0.3333f;
				SetValueNum(gm->GO_DifficultyLevel);
				break;
			case EMenuSetupName::MI_Graphics:
				SetMaxNumericValue(gm->GO_Max_GraphicsLevel);
				tu = 0.0f; tv = 0.6667f;
				SetValueNum(gm->GO_GraphicsLevel);
				break;
		}

		if (IsValid(mtiFrame)) {
			mtiFrame->SetScalarParameterValue(TEXT("tU"), tu);
			mtiFrame->SetScalarParameterValue(TEXT("tV"), tv);
			mtiFrame->SetVectorParameterValue(TEXT("ButtonColor"), vBaseColor);
		}
	}
}

void AHandMenuSlider::SetValue(float NewValue)
{
	fProgressBarValue = NewValue;
	if (mtiSlider != nullptr)
		mtiSlider->SetScalarParameterValue(TEXT("Value"), fProgressBarValue);
}

void AHandMenuSlider::Slider_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine")))
		if (mtiFrame != nullptr) {
			mtiSlider->SetVectorParameterValue(TEXT("EmissiveColor"), vActiveColor);
			ActiveButton = 1;
		};
}

void AHandMenuSlider::Slider_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine")))
		if (mtiFrame != nullptr) {
			mtiSlider->SetVectorParameterValue(TEXT("EmissiveColor"), vBaseColor);
			ActiveButton = 0;
		};
}

void AHandMenuSlider::Button_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine")))
		if (mtiFrame != nullptr) {
			mtiFrame->SetVectorParameterValue(TEXT("ButtonColor"), vActiveColor);
			ActiveButton = 2;
		};
}

void AHandMenuSlider::Button_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine")))
		if (mtiFrame != nullptr) {
			mtiFrame->SetVectorParameterValue(TEXT("ButtonColor"), vBaseColor);
			ActiveButton = 0;
		};
}

// Определение Value по точке касания луча
void AHandMenuSlider::UpdateValueByTracePoint(FVector point)
{
	float a, b, c, p, h, l;
	FVector lb, lt, rb;
	lb = SliderMesh->GetSocketLocation(TEXT("LB"));
	lt = SliderMesh->GetSocketLocation(TEXT("LT"));
	rb = SliderMesh->GetSocketLocation(TEXT("RB"));

	//DrawDebugLine(GetWorld(), lb, lt, FColor::White, false, 2.0f, 0, 1.0f);
	//DrawDebugLine(GetWorld(), lb, rb, FColor::White, false, 2.0f, 0, 1.0f);
	//DrawDebugLine(GetWorld(), lb, point, FColor::Red, false, 2.0f, 0, 1.0f);

	// треугольник lb - lt - point
	a = (point - lb).Size();
	b = (point - lt).Size();
	c = (lt - lb).Size();
	p = (a + b + c) / 2.0f;

	h = 2 * FMath::Sqrt(p * (p - a) * (p - b) * (p - c)) / c;
	l = (rb - lb).Size();

	uint8 dat;
	float datf = h / l;
	if (datf < 0.05f) datf = 0.0f;
	if (datf > 0.95f) datf = 1.0f;
	if (MaxIntVal > 1) {
		dat = (uint8)((float)MaxIntVal * (datf)) + 1;
		if (dat > MaxIntVal) dat = MaxIntVal;

		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::FromInt(dat));
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::SanitizeFloat((float)(dat / MaxIntVal)));
		SetValueNum(dat);
	}
	else {
		dat = 0;
		SetValue(datf);
	}
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::SanitizeFloat(datf));

	// update value
	AUVRPGameMode *gm = Cast<AUVRPGameMode>(GetWorld()->GetAuthGameMode());

	if (IsValid(gm)) {
		switch (MenuParam) {
			case EMenuSetupName::MS_MusicVolume:
				gm->GO_MusicVolume = datf;
				gm->SetAudioVolume(VOLUME_SC_Music, datf);
				if (IsValid(TestVolumeSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), TestVolumeSound, this->GetActorLocation());
				break;
			case EMenuSetupName::MS_SoundVolume:
				gm->GO_SoundVolume = datf;
				gm->SetAudioVolume(VOLUME_SC_Ambience, datf);
				gm->SetAudioVolume(VOLUME_SC_Sound, datf);
				if (IsValid(TestVolumeSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), TestVolumeSound, this->GetActorLocation());
				break;
			case EMenuSetupName::MS_VoiceVolume:
				gm->GO_VoiceVolume = datf;
				gm->SetAudioVolume(VOLUME_SC_Voice, datf);
				if (IsValid(TestVolumeSound)) UGameplayStatics::PlaySoundAtLocation(GetWorld(), TestVolumeSound, this->GetActorLocation());
				break;
			case EMenuSetupName::MS_DiffLevel:
				gm->SetDifficultyLevel(dat);
				break;
			case EMenuSetupName::MI_Graphics:
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "graphics level is " + FString::FromInt(dat));

				gm->SetGraphicsLevel(dat);

				//gm->ShowMessage(FText::FromString(FString::FromInt(dat)));
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, "graphics level is " + FString::FromInt(gm->GraphicsOptions.Level));
				//gm->GO_GraphicsLevel = dat;
				break;
		}
	}
}

bool AHandMenuSlider::OnClick(FVector HitPoint)
{
	if (ActiveButton == 1 && HitPoint != FVector::ZeroVector /* slider */) {
		UpdateValueByTracePoint(HitPoint);
	}
	else if (ActiveButton == 2 /* close */) {
		if (IsValid(ParentMenu)) {
			ParentMenu->SetMenuActive(true);
			ParentMenu->HasSubmenuActive = false;
		}
		Destroy();
	}
	else
		return false;

	return true;
}
