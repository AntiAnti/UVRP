// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HandMenuEmpty.h"
#include "MenuInteractionInterface.h"
#include "HandMenuSlider.generated.h"

class AHandMenuList;

UENUM(BlueprintType, Blueprintable)
enum class EMenuSetupName : uint8
{
	MS_Empty			UMETA(DisplayName = "Empty"),
	MS_SoundVolume		UMETA(DisplayName = "Sound Volume"),
	MS_DiffLevel		UMETA(DisplayName = "DiffLevel"),
	MI_Graphics			UMETA(DisplayName = "Graphics"),
	MS_MusicVolume		UMETA(DisplayName = "Music Volume"),
	MS_VoiceVolume		UMETA(DisplayName = "Voice Volume")
};

/**
 * 
 */
UCLASS(Blueprintable)
class UVRP_API AHandMenuSlider : public AHandMenuEmpty, public IMenuInteractionInterface
{
	GENERATED_BODY()

public:
	AHandMenuSlider();

	UPROPERTY(VisibleAnywhere, Category = Mesh)
	class UStaticMeshComponent* SliderMesh;

	UPROPERTY()
	UBoxComponent* SliderCollider;

	UPROPERTY()
	UBoxComponent* ButtonCollider;

	UPROPERTY()
	AHandMenuList* ParentMenu;

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetType(EMenuSetupName ItemType);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetMaxNumericValue(uint8 MaxVal) { MaxIntVal = MaxVal; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetValue(float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Global")
	void SetValueNum(uint8 NewValue) { SetValue((float)NewValue / (float)MaxIntVal); };

	UFUNCTION(BlueprintCallable, Category = "Global")
	float GetValue() { return fProgressBarValue; };

	UFUNCTION(BlueprintCallable, Category = "Global")
	void UpdateValueByTracePoint(FVector point);

	UFUNCTION(BlueprintCallable, Category = "Global")
	bool OnClick(FVector HitPoint) override;

private:
	UPROPERTY()
	UMaterialInstanceDynamic *mtiFrame;
	UPROPERTY()
	UMaterialInstanceDynamic *mtiSlider;

	UPROPERTY()
	float fProgressBarValue;

	UPROPERTY()
	FLinearColor vBaseColor;

	UPROPERTY()
	FLinearColor vActiveColor;

	UPROPERTY()
	int32 ActiveButton;

	UPROPERTY()
	EMenuSetupName MenuParam;

	UPROPERTY()
	uint8 MaxIntVal;

	UPROPERTY()
	USoundBase* TestVolumeSound;

	UFUNCTION()
	void Slider_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void Slider_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void Button_OnBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void Button_OnEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
