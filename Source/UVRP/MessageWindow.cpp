// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "UVRPCharacter.h"
#include "Engine.h"
#include "DataModuleViewList.h"
#include "MessageWindow.h"


// Sets default values
AMessageWindow::AMessageWindow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<UStaticMeshComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	WindowPlane = CreateDefaultSubobject<UStaticMeshComponent>("WindowPlane");
	WindowPlane->SetupAttachment(GetRootComponent());
	WindowPlane->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAssetPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/plane100x100.plane100x100'"));
	if (smAssetPlane.Object != NULL) {
		WindowPlane->SetStaticMesh(smAssetPlane.Object);
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> miLarge(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_MessageWindow_Large.mti_MessageWindow_Large'"));
	if (miLarge.Object != NULL) {
		mt_AssetWindowLarge = miLarge.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> miSmall(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_MessageWindow_Small.mti_MessageWindow_Small'"));
	if (miSmall.Object != NULL) {
		mt_AssetWindowSmall = miSmall.Object;
	}
	UMaterial* mt_FontMaterial;
	static ConstructorHelpers::FObjectFinder<UMaterial> miFontMaterial(TEXT("Material'/Game/FirstPerson/Effects/DefaultTextMaterialTranslucent_2.DefaultTextMaterialTranslucent_2'"));
	UFont* fnt_Main;
	static ConstructorHelpers::FObjectFinder<UFont> miFont(TEXT("Font'/Game/FirstPerson/Effects/RobotoDistanceField_2.RobotoDistanceField_2'"));

	TextComponent = CreateDefaultSubobject<UTextRenderComponent>("TextComponent");
	TextComponent->SetupAttachment(GetRootComponent());
	TextComponent->WorldSize = 6.0f;
	TextComponent->SetRelativeLocation(FVector::ZeroVector);
	TextComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	TextComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
	TextComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;
	TextComponent->SetText(FText::FromString("[DATA LIST]"));
	TextComponent->SetTextRenderColor(FColor::White);
	if (miFontMaterial.Object != NULL) {
		mt_FontMaterial = miFontMaterial.Object;
		TextComponent->SetTextMaterial(mt_FontMaterial);
		TextComponent->SetMaterial(0, mt_FontMaterial);
	}
	if (miFont.Object != NULL) {
		fnt_Main = miFont.Object;
		TextComponent->SetFont(fnt_Main);
	}

	ButtonNext = CreateDefaultSubobject<UBoxComponent>("ButtonNext"); // number 2
	ButtonNext->SetupAttachment(GetRootComponent());
	ButtonNext->SetRelativeLocation(FVector(0.0f, 32.56f, -31.07f));
	ButtonNext->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ButtonNext->SetCollisionObjectType(ECC_ControllerResponses);
	ButtonNext->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ButtonNext->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ButtonNext->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ButtonNext->SetBoxExtent(FVector(0.25f, 15.0f, 2.14f));
	ButtonNext->OnComponentBeginOverlap.AddDynamic(this, &AMessageWindow::ButtonNext_OnBeginOverlap);
	ButtonNext->OnComponentEndOverlap.AddDynamic(this, &AMessageWindow::ButtonNext_OnEndOverlap);

	ButtonBack = CreateDefaultSubobject<UBoxComponent>("ButtonBack");  // number 3
	ButtonBack->SetupAttachment(GetRootComponent());
	ButtonBack->SetRelativeLocation(FVector(0.0f, -39.63f, -31.07f));
	ButtonBack->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ButtonBack->SetCollisionObjectType(ECC_ControllerResponses);
	ButtonBack->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ButtonBack->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ButtonBack->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ButtonBack->SetBoxExtent(FVector(0.25f, 8.2f, 2.14f));
	ButtonBack->OnComponentBeginOverlap.AddDynamic(this, &AMessageWindow::ButtonBack_OnBeginOverlap);
	ButtonBack->OnComponentEndOverlap.AddDynamic(this, &AMessageWindow::ButtonBack_OnEndOverlap);
	
	ButtonClose = CreateDefaultSubobject<UBoxComponent>("ButtonClose"); // number 1
	ButtonClose->SetupAttachment(GetRootComponent());
	ButtonClose->SetRelativeLocation(FVector(0.0f, 41.66f, 31.46f));
	ButtonClose->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ButtonClose->SetCollisionObjectType(ECC_ControllerResponses);
	ButtonClose->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ButtonClose->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ButtonClose->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	ButtonClose->SetBoxExtent(FVector(0.25f, 6.8f, 2.14f));
	ButtonClose->OnComponentBeginOverlap.AddDynamic(this, &AMessageWindow::ButtonClose_OnBeginOverlap);
	ButtonClose->OnComponentEndOverlap.AddDynamic(this, &AMessageWindow::ButtonClose_OnEndOverlap);
}

// Called when the game starts or when spawned
void AMessageWindow::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMessageWindow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMessageWindow::SetWindowType(bool IsLarge, float LifeTime)
{
	IsLargeWindow = IsLarge;

	if (IsLargeWindow) {
		WindowPlane->SetMaterial(0, mt_AssetWindowLarge);
		WindowPlane->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.7f));
		ButtonClose->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ButtonBack->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		ButtonNext->SetRelativeLocation(FVector(0.0f, 32.56f, -31.07f));
		TextComponent->SetRelativeLocation(FVector(0.0f, -46.63f, 24.45f));
		TextComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextTop;
		TextComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Left;
	}
	else {
		WindowPlane->SetMaterial(0, mt_AssetWindowSmall);
		WindowPlane->SetRelativeScale3D(FVector(1.0f, 1.0f, 0.28f));
		ButtonClose->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ButtonBack->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ButtonNext->SetRelativeLocation(FVector(0.0f, 32.56f, -10.67f));
		TextComponent->SetRelativeLocation(FVector::ZeroVector);
		TextComponent->VerticalAlignment = EVerticalTextAligment::EVRTA_TextCenter;
		TextComponent->HorizontalAlignment = EHorizTextAligment::EHTA_Center;

		if (LifeTime != -1.0f)
			this->SetLifeSpan(LifeTime);
	}

	// place in front of player if actor doesn't have attached parent
	if (!IsValid(GetRootComponent()->GetAttachParent()) || !IsValid(GetRootComponent()->GetAttachParent()->GetOwner())) {
		FVector loc1, loc2;
		FRotator rot;
		AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
		loc1 = player->GetFirstPersonCameraComponent()->GetComponentLocation();
		rot = player->GetFirstPersonCameraComponent()->GetComponentRotation();
		loc2 = loc1 + player->GetFirstPersonCameraComponent()->GetForwardVector() * 120.0f;
		loc2.Z = loc1.Z;
		//if (loc2.Z - player->GetActorLocation().Z > 70.0f) loc2.Z -= 50.0f;
		rot.Roll = rot.Pitch = 0.0f;
		loc2.Z = loc1.Z;
		SetActorLocation(loc2);
		SetActorRotation(rot);

		// Attach to player
		if (!IsLargeWindow) {
			APawn* player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
			if (IsValid(player)) {
				this->AttachToActor(player, FAttachmentTransformRules::KeepWorldTransform);
			}
		}
	}

	mti_Window = WindowPlane->CreateDynamicMaterialInstance(0);
}

void AMessageWindow::SetTextData(TArray<FText> Data)
{
	TextData = Data;
	CurrentPage = 1;
	UpdateText();
}

void AMessageWindow::UpdateText()
{
	const float windowsize = 94.0f;

	if (CurrentPage >= 1 && CurrentPage <= TextData.Num()) {
		TextComponent->WorldSize = 6.0f;
		TextComponent->SetText(TextData[CurrentPage - 1]);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, TextData[CurrentPage - 1].ToString());

		FVector size = TextComponent->GetTextLocalSize();
		float coef = windowsize / size.Y;

		if (coef < 1.0f)
			TextComponent->WorldSize = 6.0f * coef;
		else
			TextComponent->WorldSize = 6.0f;
	}
}

bool AMessageWindow::OnClick(FVector HitPoint)
{
	OnButtonClick.Broadcast(ActiveButton);

	if (ActiveButton == 1 || (ActiveButton == 2 && !IsLargeWindow)) {
		if (IsValid(OwnerMenu)) {
			OwnerMenu->OnDataWindowClosed();
		}

		Destroy();
	}
	else if (ActiveButton == 2 && IsLargeWindow) {
		// next page
		if (CurrentPage < TextData.Num()) CurrentPage++;
		UpdateText();
	}
	else if (ActiveButton == 3) {
		// prev page
		if (CurrentPage > 1) CurrentPage--;
		UpdateText();
	}

	return true;
}

void AMessageWindow::ButtonClose_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 1.0f);
	ActiveButton = 1;
}

void AMessageWindow::ButtonClose_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 0.0f);
	ActiveButton = 0;
}

void AMessageWindow::ButtonNext_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IsLargeWindow || CurrentPage < TextData.Num()) {
		if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 2.0f);
		ActiveButton = 2;
	}
}

void AMessageWindow::ButtonNext_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 0.0f);
	ActiveButton = 0;
}

void AMessageWindow::ButtonBack_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (CurrentPage > 1) {
		if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 3.0f);
		ActiveButton = 3;
	}
}

void AMessageWindow::ButtonBack_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(mti_Window)) mti_Window->SetScalarParameterValue(TEXT("Level"), 0.0f);
	ActiveButton = 0;
}
