// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#ifndef A_HAND_MENU_LIST_MAXITEMS
#define A_HAND_MENU_LIST_MAXITEMS 10
//#endif

#include "HandMenuEmpty.h"
#include "HandMenuItem.h"
#include "HandMenuList.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API AHandMenuList : public AHandMenuEmpty, public IMenuInteractionInterface
{
	GENERATED_BODY()

public:
	AHandMenuList();

	virtual void BeginPlay() override;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TSubobjectPtrDeprecated<UHandMenuItem> MenuItemComponent[A_HAND_MENU_LIST_MAXITEMS];

	TSubobjectPtrDeprecated<UBoxComponent> MenuItemCollider[A_HAND_MENU_LIST_MAXITEMS];

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	TArray<FMenuTreeItem> CurrentMenuList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	FName CurrentSocketName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global")
	bool HasSubmenuActive;

public:
	UFUNCTION(BlueprintCallable, Category = "Global")
	bool OnClick(FVector HitPoint) override;

	UFUNCTION(BlueprintCallable, Category = "Global")
	void Navigate(int32 ItemIndex);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetMenuActive(bool IsActive);

protected:
	UPROPERTY()
	int32 ActiveMenuItem;

private:
	UFUNCTION()
	void OnBeginOverlap0(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) { OnBeginOverlapButton(0, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap0(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(0, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap1(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(1, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap1(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(1, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap2(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(2, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap2(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(2, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap3(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(3, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap3(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(3, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap4(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(4, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap4(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(4, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap5(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(5, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap5(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(5, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap6(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(6, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap6(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(6, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap7(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(7, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap7(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(7, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap8(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(8, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap8(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(8, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlap9(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)  { OnBeginOverlapButton(9, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult); };
	UFUNCTION()
	void OnEndOverlap9(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) { OnEndOverlapButton(9, OtherActor, OtherComp, OtherBodyIndex); };
	UFUNCTION()
	void OnBeginOverlapButton(int32 Index, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapButton(int32 Index, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
