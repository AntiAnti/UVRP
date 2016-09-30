// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "HandMenuEmpty.h"

// Sets default values
AHandMenuEmpty::AHandMenuEmpty()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PlaneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("PlaneMesh"));
	SetRootComponent(PlaneRoot);
	//PlaneRoot->RelativeLocation = FVector(0.f, 0.f, 0.f);

	//mtiMenu = PlaneMesh->CreateDynamicMaterialInstance(0);
}

// Called when the game starts or when spawned
void AHandMenuEmpty::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHandMenuEmpty::Tick( float DeltaTime )
{
	Super::Tick(DeltaTime);
}

/*
void AHandMenuEmpty::Show()
{
	FTimerDelegate MyDel;
	MyDel.BindRaw(this, &AHandMenuEmpty::AHandMenuEmpty);
	GetWorldTimerManager().SetTimer(hShowHide, MyDel, 5.f, false);
}

void AHandMenuEmpty::Hide()
{

}
*/

float AHandMenuEmpty::GetItemHeight()
{
	return 13.0f;
}

void AHandMenuEmpty::ShowFinished_Implementation()
{
	return;
}

void AHandMenuEmpty::HideFinished_Implementation()
{
	return;
}

void AHandMenuEmpty::OnOpenItem_Implementation(FMenuTreeItem item)
{
	return;
}

void AHandMenuEmpty::OnExitMenu_Implementation()
{
	return;
}

void AHandMenuEmpty::TimerSetOpacity()
{

}