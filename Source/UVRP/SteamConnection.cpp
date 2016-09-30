// Anton Antonov, 2016. All rights reserved.

#include "UVRP.h"
//#include "Online.h"
#include "UVRPGameMode.h"
#include "SteamConnection.h"

USteamConnection::USteamConnection()
{
}

USteamConnection::~USteamConnection()
{
}

/*
IOnlineSubsystem* USteamConnection::InitSubsystem()
{
	return IOnlineSubsystem::Get(FName("Steam"));
}
*/

bool USteamConnection::IsPlayerLoggedIn()
{
	/*
	int32 id;
	bool bRet;
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
	if (SteamSubsystem != nullptr) {
		APlayerController* ctrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(ctrl)) id = ctrl->GetLocalPlayer()->GetControllerId();

		bRet = true;
	}
	else bRet = false;

	return bRet;
	*/
	return false;
}

void USteamConnection::UnlockAchievement(EGameAchievements AchievementType)
{
	/*
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
	FName AchName;
	if (SteamSubsystem != nullptr) {
		int32 id = 0;
		APlayerController* ctrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(ctrl)) id = ctrl->GetLocalPlayer()->GetControllerId(); // == 0
		//ctrl->GetLocalPlayer()->

		switch (AchievementType) {
			case EGameAchievements::A_GameEnded:
				AchName = TEXT("ACH_GAME_ENDED"); break;
			case EGameAchievements::A_ModuleDecrypted:
				AchName = TEXT("ACH_MODULE_DECRYPTED"); break;
			case EGameAchievements::A_ExtendedEnding:
				AchName = TEXT("ACH_EXTENDED_END"); break;
			case EGameAchievements::A_SecretArea1:
				AchName = TEXT("ACH_SECRET_AREA"); break;
			case EGameAchievements::A_BothSecretAreas:
				AchName = TEXT("ACH_ALL_SECRET_AREAS"); break;
			case EGameAchievements::A_HighestDifficulty:
				AchName = TEXT("ACH_MAX_DIFFICULTY"); break;
			default:
				return;
		}
		
		IOnlineAchievementsPtr i_Ach = SteamSubsystem->GetAchievementsInterface();		
		FOnlineAchievementsWriteRef obj = MakeShareable(new FOnlineAchievementsWrite());
		obj->SetIntStat(AchName, 1);
		
		//i_Ach->WriteAchievements( id, obj);
	}
	*/
}

void USteamConnection::LoadAchievementStatus(EGameAchievements AchievementType, bool& ResultVariable)
{
	/*
	FName AchName;
	switch (AchievementType) {
	case EGameAchievements::A_GameEnded:
		AchName = TEXT("ACH_GAME_ENDED"); break;
	case EGameAchievements::A_ModuleDecrypted:
		AchName = TEXT("ACH_MODULE_DECRYPTED"); break;
	case EGameAchievements::A_ExtendedEnding:
		AchName = TEXT("ACH_EXTENDED_END"); break;
	case EGameAchievements::A_SecretArea1:
		AchName = TEXT("ACH_SECRET_AREA"); break;
	case EGameAchievements::A_BothSecretAreas:
		AchName = TEXT("ACH_ALL_SECRET_AREAS"); break;
	case EGameAchievements::A_HighestDifficulty:
		AchName = TEXT("ACH_MAX_DIFFICULTY"); break;
	default:
		return;
	}
	*/

	/*
	IOnlineSubsystem* SteamSubsystem = IOnlineSubsystem::Get(FName("Steam"));
	if (SteamSubsystem != nullptr) {
		int32 id = 0;
		APlayerController* ctrl = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(ctrl)) id = ctrl->GetLocalPlayer()->GetControllerId(); // == 0

		IOnlineAchievementsPtr i_Ach = SteamSubsystem->GetAchievementsInterface();
		
		i_Ach->QueryAchievements(id, FOnQueryAchievementsCompleteDelegate());
	}
	*/
}
