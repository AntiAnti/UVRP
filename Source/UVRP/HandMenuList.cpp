// Fill out your copyright notice in the Description page of Project Settings.

#include "UVRP.h"
#include "Engine.h"
#include "HandMenuSlider.h"
#include "MenuInteractionInterface.h"
#include "HandMenuList.h"
#include "UVRPGameMode.h"
#include "UVRPCharacter.h"

AHandMenuList::AHandMenuList()
{
	// initialize material
	if (!MainMaterialInstance) {
		static ConstructorHelpers::FObjectFinder<UMaterialInstance> mtrlAsset(TEXT("MaterialInstanceConstant'/Game/FirstPerson/Shaders/Materials/mti_HandMenu1.mti_HandMenu1'"));
		if (mtrlAsset.Object != NULL) {
			MainMaterialInstance = (UMaterialInstance*)mtrlAsset.Object;
		}
	}

	for (int32 i = 0; i < A_HAND_MENU_LIST_MAXITEMS; i++) {
		FString ind;
		ind = TEXT("MenuButton") + FString::FromInt(i);
		FName CompName = FName(TCHAR_TO_ANSI(*ind));
		ind = TEXT("MenuButtonCol") + FString::FromInt(i);
		FName ColliderName = FName(TCHAR_TO_ANSI(*ind));

		const float ItemHeight = UHandMenuItem::ScreenHeight;
		// button mesh
		MenuItemComponent[i] = CreateDefaultSubobject<UHandMenuItem>(CompName);
		MenuItemComponent[i]->SetupAttachment(GetRootComponent());
		MenuItemComponent[i]->SetRelativeLocation(
				FVector(0.0f, 0.0f, ItemHeight * i));
		//MenuItemComponent[i]->SetMaterial(1, MainMaterialInstance);
		MenuItemComponent[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MenuItemComponent[i]->SetVisibility(false);

		// button collider
		MenuItemCollider[i] = CreateDefaultSubobject<UBoxComponent>(ColliderName);
		MenuItemCollider[i]->SetupAttachment(GetRootComponent());
		MenuItemCollider[i]->SetRelativeLocation(
			FVector(25.0f, -3.0f, ItemHeight/2 + ItemHeight * i) + 3.0f);
		MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MenuItemCollider[i]->SetCollisionObjectType(ECC_ControllerResponses);
		MenuItemCollider[i]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);		
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
		MenuItemCollider[i]->SetBoxExtent(FVector(25.0f, 0.2f, ItemHeight/2));
		MenuItemCollider[i]->bHiddenInGame = true;

		switch (i) {
		case 0:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap0);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap0);
			break;
		case 1:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap1);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap1);
			break;
		case 2:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap2);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap2);
			break;
		case 3:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap3);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap3);
			break;
		case 4:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap4);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap4);
			break;
		case 5:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap5);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap5);
			break;
		case 6:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap6);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap6);
			break;
		case 7:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap7);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap7);
			break;
		case 8:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap8);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap8);
			break;
		case 9:
			MenuItemCollider[i]->OnComponentBeginOverlap.AddDynamic(this, &AHandMenuList::OnBeginOverlap9);
			MenuItemCollider[i]->OnComponentEndOverlap.AddDynamic(this, &AHandMenuList::OnEndOverlap9);
			break;
		}
	}

	/*
	CurrentMenuList.Empty();
	FMenuTreeItem root;
	root.Item = EMenuItemName::MI__ROOT;
	root.ParentIndex = -1;
	CurrentMenuList.Add(root);
	*/
	ActiveMenuItem = -1;
	CurrentSocketName = "";
	MenuPath = 0;
	Navigate(MenuPath);
}

// Called when the game starts or when spawned
void AHandMenuList::BeginPlay()
{
	Super::BeginPlay();

	for (int32 i = 0; i < A_HAND_MENU_LIST_MAXITEMS; i++) {
		FString ind;
		//MenuItemComponent[i]->SetWorldLocation(FVector::ZeroVector);
		MenuItemComponent[i]->SetRelativeLocation(
			FVector(0.0f, 0.0f, UHandMenuItem::ScreenHeight * i));

		MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MenuItemCollider[i]->SetCollisionObjectType(ECC_ControllerResponses);
		MenuItemCollider[i]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
		MenuItemCollider[i]->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
	}

	Navigate(MenuPath);
}

void AHandMenuList::Navigate(int32 ItemIndex)
{
	MenuPath = ItemIndex;

	// Update list
	CurrentMenuList.Empty();
	for (int i = MenuTree.Num() - 1; i>=0; i--) {
		if (MenuTree[i].ParentIndex == MenuPath) {
			FMenuTreeItem item;
			item = MenuTree[i];
			item.ParentIndex = i;
			CurrentMenuList.Add(item);
		}
	}

	// Update components
	for (int i = 0; i < A_HAND_MENU_LIST_MAXITEMS; i++) {
		MenuItemComponent[i]->SetVisibility(i < CurrentMenuList.Num());
		if (i < CurrentMenuList.Num()) {
			MenuItemComponent[i]->SetItem(CurrentMenuList[i].Item, CurrentMenuList[i].Type);

			MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			if (CurrentMenuList[i].Type == EMenuItemType::IT_Title)
				MenuItemComponent[i]->SetItemColor(1);
			else if (CurrentMenuList[i].Item == EMenuItemName::MI_On || CurrentMenuList[i].Item == EMenuItemName::MI_Off || CurrentMenuList[i].Item == EMenuItemName::MI_Automatic) {
				EMenuItemName titleitem = EMenuItemName::MI_SetupHands;

				// узнать какую переменную устанавливаем
				for (int titleindex = ActiveMenuItem; titleindex < CurrentMenuList.Num(); titleindex++)
					if (CurrentMenuList[titleindex].Type == EMenuItemType::IT_Title) { titleitem = CurrentMenuList[titleindex].Item; break; };

				if (titleitem == EMenuItemName::MI_SetupHands) {
					AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(gm)) {
						if ((gm->GO_ShowHands && CurrentMenuList[i].Item == EMenuItemName::MI_On) || (!gm->GO_ShowHands && CurrentMenuList[i].Item == EMenuItemName::MI_Off))
							MenuItemComponent[i]->SetItemColor(2);
						else
							MenuItemComponent[i]->SetItemColor(0);
					}
				}
				else if (titleitem == EMenuItemName::MI_SetupGraphics) {
					AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(gm)) {
						if (gm->IsAutoFPSCheckActive())
							MenuItemComponent[i]->SetItemColor(2);
						else
							MenuItemComponent[i]->SetItemColor(0);
					}
				}
				else if (titleitem == EMenuItemName::MI_Learning) {
					AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(gm)) {
						if ((gm->GO_GameplayTutorial && CurrentMenuList[i].Item == EMenuItemName::MI_On) || (!gm->GO_GameplayTutorial && CurrentMenuList[i].Item == EMenuItemName::MI_Off))
							MenuItemComponent[i]->SetItemColor(2);
						else
							MenuItemComponent[i]->SetItemColor(0);
					}
				}
			}
			else
				MenuItemComponent[i]->SetItemColor(0);
		}
		else {
			MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AHandMenuList::SetMenuActive(bool IsActive)
{
	this->SetActorHiddenInGame(!IsActive);

	if (IsActive) {
		for (int i = 0; i < A_HAND_MENU_LIST_MAXITEMS; i++) {
			MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

			MenuItemCollider[i]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
			MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
			MenuItemCollider[i]->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
			MenuItemCollider[i]->SetCollisionResponseToChannel(ECC_ControllerResponses, ECollisionResponse::ECR_Block);
		}
	}
	else {
		for (int i = 0; i < A_HAND_MENU_LIST_MAXITEMS; i++) {
			MenuItemCollider[i]->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			MenuItemCollider[i]->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		}
	}
}

bool AHandMenuList::OnClick(FVector HitPoint)
{
	bool bRet = false;


	if (ActiveMenuItem < 0) return bRet;
	if (ActiveMenuItem >= A_HAND_MENU_LIST_MAXITEMS) ActiveMenuItem = A_HAND_MENU_LIST_MAXITEMS;
	int index = CurrentMenuList[ActiveMenuItem].ParentIndex;
	if (!MenuItemComponent[ActiveMenuItem]->IsVisible()) return bRet;

	// в зависимости от типа элемента выбираем действие
	// action - какое-то определенное действие
	// folder - показать подэлементы
	// back - предыдущий уровень
	// parameter - показать окно задания параметров
	// title - ничего

	/************************************** FOLDERS AND BACK **************************************/
	if (CurrentMenuList[ActiveMenuItem].Type == EMenuItemType::IT_Folder || CurrentMenuList[ActiveMenuItem].Type == EMenuItemType::IT_Back) {
		//OnOpenItem(MenuTree[index]);
		if (ActiveMenuItem >= 0 && ActiveMenuItem < CurrentMenuList.Num()) {
			if (MenuTree[index].Type == EMenuItemType::IT_Back) {
				index = MenuTree[MenuTree[index].ParentIndex].ParentIndex;
			}

			if (index < 0) { OnExitMenu(); index = 0; }

			Navigate(index);

			if (ActiveMenuItem >= CurrentMenuList.Num())
				MenuItemComponent[ActiveMenuItem]->SetItemActive(false);
			/*
			if (ActiveMenuItem >= 0 && ActiveMenuItem < A_HAND_MENU_LIST_MAXITEMS && !MenuItemComponent[ActiveMenuItem]->IsVisible())
				ActiveMenuItem = -1;
			*/
		}
	/******************************************* ACTIONS ******************************************/
	} else if (CurrentMenuList[ActiveMenuItem].Type == EMenuItemType::IT_Action) {
		int32 titleindex;

		/* * * * * * * * * * * * * * * * * * * * * MAIN MENU * * * * * * * * * * * * * * * * * * * * */
		if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_MainMenu || CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_Continue) {
			AUVRPCharacter* player = Cast<AUVRPCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());

			if (IsValid(player)) {
				if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_MainMenu) { //player->IsInMainMenu() && player->GetIsGameActive()) {
					player->OpenMainMenu();
				}
				else {
					player->ReturnFromMainMenu();
				}
				OnExitMenu();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * STATISTICS * * * * * * * * * * * * * * * * * * * * */
		else if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_Statistics) {
			AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

			if (IsValid(gm)) {
				if (UGameplayStatics::GetCurrentLevelName(GetWorld()) == "trainingroom") {
					//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Max record: " + FString::FromInt(gm->GP_TrainingLevelScore));
					gm->showstat();
				}
				else {
					gm->showstat();
					/*
					AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
					if (IsValid(player)) player->SetupGameplayTime(false);
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Soldiers killed: " + FString::FromInt(gm->GameStat.Killed));
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Soldiers injured: " + FString::FromInt(gm->GameStat.Injured));
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Player died times: " + FString::FromInt(gm->GameStat.Died));
					if (gm->GameStat.TeleportsUnlocked > 0)
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Teleports unlocking efficiency: " + FString::SanitizeFloat(gm->GameStat.CodesReceived / gm->GameStat.TeleportsUnlocked));
					else
						GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Teleports unlocking efficiency: none");
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "In-game time: " + FString::SanitizeFloat(gm->GameStat.InGameTime) + " seconds");
					*/
				}
				OnExitMenu();
			}
		}

		/* * * * * * * * * * * * * * * * * * * * * FLOOE CALIBRATION * * * * * * * * * * * * * * * * * * * * */
		else if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_Floor) {
			AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

			if (IsValid(gm)) {
				gm->CalibrateFloor();
			}
			SetMenuActive(false);
			//OnExitMenu();
		}

		/* * * * * * * * * * * * * * * * * * * * * ON AND OFF * * * * * * * * * * * * * * * * * * * * */
		else if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_On || CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_Off) {
			bool newvalue = (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_On);
			EMenuItemName titleitem = EMenuItemName::MI_SetupHands;

			// узнать какую переменную устанавливаем
			for (titleindex = ActiveMenuItem; titleindex < CurrentMenuList.Num(); titleindex++)
				if (CurrentMenuList[titleindex].Type == EMenuItemType::IT_Title) { titleitem = CurrentMenuList[titleindex].Item; break; };

			// установить ей новое значение
			AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			AUVRPCharacter* player = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (titleindex >= 0 && IsValid(gm) && IsValid(player)) {
				if (titleitem == EMenuItemName::MI_SetupHands) {
					gm->GO_ShowHands = newvalue;
					player->UpdateHandsView(gm->GO_ShowHands);
				}
				else if (titleitem == EMenuItemName::MI_Learning) {
					gm->GO_GameplayTutorial = newvalue;
				}
			}
			MenuItemComponent[ActiveMenuItem]->SetItemColor(2);
			if (ActiveMenuItem > 0 && (MenuItemComponent[ActiveMenuItem-1]->GetItem() == EMenuItemName::MI_On || MenuItemComponent[ActiveMenuItem - 1]->GetItem() == EMenuItemName::MI_Off))
				MenuItemComponent[ActiveMenuItem - 1]->SetItemColor(0);
			else if (ActiveMenuItem < A_HAND_MENU_LIST_MAXITEMS-1 && (MenuItemComponent[ActiveMenuItem + 1]->GetItem() == EMenuItemName::MI_On || MenuItemComponent[ActiveMenuItem + 1]->GetItem() == EMenuItemName::MI_Off))
				MenuItemComponent[ActiveMenuItem + 1]->SetItemColor(0);
		}

		/* * * * * * * * * * * * * * * * * * * * * AUTOMATIC * * * * * * * * * * * * * * * * * * * * */
		else if (CurrentMenuList[ActiveMenuItem].Item == EMenuItemName::MI_Automatic) {
			EMenuItemName titleitem = EMenuItemName::MI_SetupGraphics;
			// узнать какую переменную устанавливаем
			for (titleindex = ActiveMenuItem; titleindex < CurrentMenuList.Num(); titleindex++)
				if (CurrentMenuList[titleindex].Type == EMenuItemType::IT_Title) { titleitem = CurrentMenuList[titleindex].Item; break; };


			AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (titleindex >= 0 && IsValid(gm) && titleitem == EMenuItemName::MI_SetupGraphics) {
				gm->SetAutoFPSCheck(!gm->IsAutoFPSCheckActive());
				if (gm->IsAutoFPSCheckActive())
					MenuItemComponent[ActiveMenuItem]->SetItemColor(2);
				else
					MenuItemComponent[ActiveMenuItem]->SetItemColor(0);
				//SetMenuActive(false);
				//OnExitMenu();
			}
		}
	/****************************************** PARAMETERS ****************************************/
	} else if (CurrentMenuList[ActiveMenuItem].Type == EMenuItemType::IT_Parameter) {
		FTransform spT = MenuItemCollider[ActiveMenuItem]->GetComponentTransform();
		FActorSpawnParameters params;
		params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AHandMenuSlider *slider = GetWorld()->SpawnActor<AHandMenuSlider>(AHandMenuSlider::StaticClass(), spT, params);

		FString p;
		CurrentSocketName.ToString(p);

		EMenuItemName itname = CurrentMenuList[ActiveMenuItem].Item;
		if (itname == EMenuItemName::MI_Manual) {
			// узнать какую переменную устанавливаем
			for (int titleindex = ActiveMenuItem; titleindex < CurrentMenuList.Num(); titleindex++)
				if (CurrentMenuList[titleindex].Type == EMenuItemType::IT_Title) {
					itname = CurrentMenuList[titleindex].Item; break;
				};
		}
		
		if (IsValid(slider)) {
			switch (itname) {
				case EMenuItemName::MI_SetupMusic: slider->SetType(EMenuSetupName::MS_MusicVolume); break;
				case EMenuItemName::MI_SetupVoice: slider->SetType(EMenuSetupName::MS_VoiceVolume); break;
				case EMenuItemName::MI_SetupSound: slider->SetType(EMenuSetupName::MS_SoundVolume); break;
				case EMenuItemName::MI_SetupDiffLevel: slider->SetType(EMenuSetupName::MS_DiffLevel); break;
				case EMenuItemName::MI_SetupGraphics:
					slider->SetType(EMenuSetupName::MI_Graphics);
					AUVRPGameMode* gm = Cast<AUVRPGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
					if (IsValid(gm)) gm->SetAutoFPSCheck(false);

					// сбросить цвет соседней ячейки
					if (ActiveMenuItem > 0 && MenuItemComponent[ActiveMenuItem - 1]->GetItem() == EMenuItemName::MI_Automatic)
						MenuItemComponent[ActiveMenuItem - 1]->SetItemColor(0);
					else if (ActiveMenuItem < A_HAND_MENU_LIST_MAXITEMS - 1 && MenuItemComponent[ActiveMenuItem + 1]->GetItem() == EMenuItemName::MI_Automatic)
						MenuItemComponent[ActiveMenuItem + 1]->SetItemColor(0);

					break;
			}
			if (CurrentSocketName != TEXT("None")) {
				AUVRPCharacter *pawn = Cast<AUVRPCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
				if (pawn != nullptr) {
					slider->AttachToActor(pawn, FAttachmentTransformRules::KeepRelativeTransform);
					pawn->AttachActorToHand(slider, CurrentSocketName);
					slider->SetActorRelativeLocation(FVector::ZeroVector);
					slider->SetActorRelativeRotation(FRotator(-0.0f, 0.0f /*yaw*/, 0.0f));
				}
				else {
					slider->AttachToActor(GetAttachParentActor(), FAttachmentTransformRules::KeepRelativeTransform, CurrentSocketName);
				}
			}
			else {
				slider->SetActorLocation(MenuItemCollider[ActiveMenuItem]->GetComponentLocation());
				slider->AddActorLocalOffset(FVector(-20.0f, 0.0f, -20.0f));
				slider->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
				slider->ParentMenu = this;
				SetMenuActive(false);
			}
			HasSubmenuActive = true;
		}
		//slider->SetActorRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

		//OnOpenItem(MenuTree[index]);
		OnExitMenu();
	} else {
		bRet = false;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::FromInt(ActiveMenuItem));

		//OnOpenItem(MenuTree[index]);
	}

	return bRet;
}

void AHandMenuList::OnBeginOverlapButton(int32 Index, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Index < CurrentMenuList.Num() && MenuItemComponent[Index]->IsVisible()) {
		if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine"))) {
			ActiveMenuItem = Index;

			for (int i = 0; i < CurrentMenuList.Num(); i++)
				MenuItemComponent[i]->SetItemActive(i == Index);
		}
	}
}

void AHandMenuList::OnEndOverlapButton(int32 Index, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Index < CurrentMenuList.Num() && ActiveMenuItem == Index) {
		if (OtherActor->ActorHasTag(TEXT("ControllerToolLine")) || OtherComp->ComponentHasTag(TEXT("ControllerToolLine"))) {
			ActiveMenuItem = -1;
			MenuItemComponent[Index]->SetItemActive(false);
		}
	}
}