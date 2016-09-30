// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "MenuInteractionInterface.h"
#include "Components/TextRenderComponent.h"
#include "MessageWindow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWindowButton, uint8, ButtonNumber);

class ADataModuleViewList;

UCLASS(Blueprintable)
class UVRP_API AMessageWindow : public AActor, public IMenuInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMessageWindow();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	UStaticMeshComponent* WindowPlane;

	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	UBoxComponent* ButtonClose;

	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	UBoxComponent* ButtonNext;

	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	UBoxComponent* ButtonBack;

	UPROPERTY(VisibleAnywhere, Category = "Message Window")
	UTextRenderComponent* TextComponent;

public:
	UFUNCTION(BlueprintCallable, Category = "Message Window")
	void SetWindowType(bool IsLarge, float LifeTime = 20.0f);

	UFUNCTION(BlueprintCallable, Category = "Message Window")
	void SetTextData(TArray<FText> Data);

	UFUNCTION(BlueprintCallable, Category = "Message Window")
	void CloseWindow() {};

	UFUNCTION(BlueprintCallable, Category = "Message Window")
	void SetOwnerMenu(ADataModuleViewList* OwnerMenuActor) { OwnerMenu = OwnerMenuActor; };

	UPROPERTY(BlueprintAssignable)
	FMessageWindowButton OnButtonClick;

protected:
	UPROPERTY()
	TArray<FText> TextData;

	UPROPERTY()
	bool IsLargeWindow;

	UPROPERTY()
	int32 CurrentPage;

	UPROPERTY()
	uint8 ActiveButton;

	UPROPERTY()
	UMaterialInstance* mt_AssetWindowLarge;

	UPROPERTY()
	UMaterialInstance* mt_AssetWindowSmall;

	UPROPERTY()
	UMaterialInstanceDynamic* mti_Window;

	UPROPERTY()
	ADataModuleViewList* OwnerMenu;

	UFUNCTION()
	void UpdateText();

	UFUNCTION()
	bool OnClick(FVector HitPoint) override;

public:
	UFUNCTION()
	void ButtonClose_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ButtonClose_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ButtonNext_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ButtonNext_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void ButtonBack_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void ButtonBack_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};