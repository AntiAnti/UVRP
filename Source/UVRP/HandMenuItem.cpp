// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "HandMenuItem.h"

UHandMenuItem::UHandMenuItem()
{
	// initialize mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> smAssetPlane(TEXT("StaticMesh'/Game/FirstPerson/Meshes/menu_plane.menu_plane'"));
	if (smAssetPlane.Object != NULL) {
		SetStaticMesh(smAssetPlane.Object);	
	}

	// initialize menu
	ItemType = EMenuItemName::MI_MainMenu;
}

void UHandMenuItem::BeginPlay()
{
	if (!mtiMenu) {
		mtiMenu = CreateDynamicMaterialInstance(0);
		//mtiMenu->SetScalarParameterValue(FName("tV"), 0.5f);
	}
}

void UHandMenuItem::SetItem(EMenuItemName item, EMenuItemType type)
{
	if (!mtiMenu) {
		mtiMenu = CreateDynamicMaterialInstance(0);
		//mtiMenu->SetScalarParameterValue(FName("tV"), 0.5f);
	}

	ItemType = item;
	ItemTypeVal = type;
	if (item != EMenuItemName::MI__ROOT && mtiMenu != nullptr) {
		float u, v;
		int t;
		GetUV(item, u, v, t);

		mtiMenu->SetScalarParameterValue(FName("tU"), u);
		mtiMenu->SetScalarParameterValue(FName("tV"), v);
		mtiMenu->SetScalarParameterValue(FName("TextureIndex"), (float)t);
	}
}

void UHandMenuItem::GetUV(EMenuItemName item, float &u, float &v, int &texture)
{
	const float TextureWidth = 1024.0f;
	const float TexItemWidth = 512.0f;
	const float TexItemHeight = 137.0f;
	const float RowsCount = 7;
	const float ColsCount = 2;

	int index = -1, texind = 0;
	if (item == EMenuItemName::MI_NewGame) {
		index = 0;
	} 
	else if (item == EMenuItemName::MI_Options) {
		index = 2;
	}
	else if (item == EMenuItemName::MI_Statistics) {
		index = 13;
	}
	else if (item == EMenuItemName::MI_Learning) {
		index = 12;
	}
	else if (item == EMenuItemName::MI_MainMenu) {
		index = 0;
	}
	else if (item == EMenuItemName::MI_SetupHands) {
		index = 3;
	}
	else if (item == EMenuItemName::MI_SetupGraphics) {
		index = 4;
	}
	else if (item == EMenuItemName::MI_SetupDiffLevel) {
		index = 5;
	}
	else if (item == EMenuItemName::MI_SetupSound) {
		index = 2; texind = 1;
	}
	else if (item == EMenuItemName::MI_SetupVoice) {
		index = 3; texind = 1;
	}
	else if (item == EMenuItemName::MI_SetupVolume) {
		index = 6;
	}
	else if (item == EMenuItemName::MI_Continue) {
		index = 1;
	}
	else if (item == EMenuItemName::MI_Cancel) {
		index = 10;
	}
	else if (item == EMenuItemName::MI_Back) {
		index = 11;
	}
	else if (item == EMenuItemName::MI_On) {
		index = 5; texind = 1;
	}
	else if (item == EMenuItemName::MI_Off) {
		index = 0; texind = 1;
	}
	else if (item == EMenuItemName::MI_SetupMusic) {
		index = 7; texind = 0;
	}
	else if (item == EMenuItemName::MI_Manual) {
		index = 8; texind = 0;
	}
	else if (item == EMenuItemName::MI_Automatic) {
		index = 9; texind = 0;
	}
	else if (item == EMenuItemName::MI_Subtitles) {
		index = 4; texind = 1;
	}
	else if (item == EMenuItemName::MI_Floor) {
		index = 6; texind = 1;
	}

	if (index >= 0) {
		v = index % (int)RowsCount;
		u = (index - v) / RowsCount;

		u /= ColsCount;
		v *= (TexItemHeight / TextureWidth); // /= RowsCount;
		texture = texind;
	}
	else u = v = 0.0f;
}

void UHandMenuItem::SetItemActive(bool NewState)
{
	if (!mtiMenu || ItemTypeVal == EMenuItemType::IT_Title) return;

	if (NewState)
		mtiMenu->SetScalarParameterValue(FName("Value"), 1.0f);
	else
		mtiMenu->SetScalarParameterValue(FName("Value"), 0.0f);
}

void UHandMenuItem::SetItemColor(uint8 ItemType)
{
	FLinearColor c;
	switch (ItemType) {
	case 1:
		c = FLinearColor(0.335f, 0.335f, 0.335f, 1.0f); break;
	case 2:
		c = FLinearColor(0.325f, 0.107f, 0.076f, 1.0f); break;
	case 0:
	default:
		c = FLinearColor(0.009f, 0.238f, 0.371f, 1.0f); break;
	}

	mtiMenu->SetVectorParameterValue(TEXT("MainColor"), c);
}
