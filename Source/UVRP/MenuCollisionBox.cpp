// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
//#include "MenuInteractionInterface.h"
#include "MenuCollisionBox.h"


// Sets default values
AMenuCollisionBox::AMenuCollisionBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
	//SceneRoot->RelativeLocation = FVector(0.f, 0.f, 0.f);

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetupAttachment(GetRootComponent());
	BoxCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollider->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void AMenuCollisionBox::BeginPlay()
{
	Super::BeginPlay();	
}

// Called every frame
void AMenuCollisionBox::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AMenuCollisionBox::SetTerminalLevel()
{
	OnBoxClicked(TargetTerminalLevel);
}

void AMenuCollisionBox::OnBoxClicked_Implementation(int32 Level)
{
	return;
}