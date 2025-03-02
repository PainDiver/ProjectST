// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerController.h"
#include "Data/DataTableManager.h"
#include "Misc/GlobalMacros.h"
#include "Data/DataStructs/Data_AccountData.h"
#include "Data/AccountManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/GameModeBase.h"
#include "Game/GameMode/GameSession/STGameSessionInterface.h"
#include "GameFramework/GameSession.h"


ASTPlayerControllerBase::ASTPlayerControllerBase()
	:APlayerController()
{
	bReplicates = true;
}


ASTPlayerControllerBase_StandAlone::ASTPlayerControllerBase_StandAlone()
	:ASTPlayerControllerBase()
{
}


ASTPlayerController_ServerBase::ASTPlayerController_ServerBase()
	:ASTPlayerControllerBase()
{

}

void ASTPlayerController_ServerBase::ProcessSpawning_Implementation()
{
	ConfirmSpawning_Client();
}

void ASTPlayerController_ServerBase::ConfirmSpawning_Client_Implementation()
{
	//UserAccount로부터 Data
	UAccountManager* AccountManager = UAccountManager::GetAccountManager();
	if (AccountManager == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AccountManager Not Found"));
		return;
	}


	FAccountData LocalAccountData;
	// 
	//클라의 AccountData
	if (DummyAccountData.UserUID != -1)
	{
		LocalAccountData = DummyAccountData;
	}
	else
	{
		LocalAccountData = AccountManager->GetLocalAccountData();
		if (LocalAccountData.UserUID == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("USER UID Not Found"));
			return;
		}
	}
	ProcessSpawning_Server(LocalAccountData);
}

void ASTPlayerController_ServerBase::ProcessSpawning_Server_Implementation(const FAccountData& AccountData)
{
	// GameMode의 Validation 필요
	if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld()))
	{
		ISTGameSessionInterface::Execute_FinishSpawning(GameMode->GameSession,this,AccountData);
	}
}

