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
#include "Misc/STEventManager.h"
#include "Character/Component/STCheatComponent.h"

ASTPlayerControllerBase::ASTPlayerControllerBase()
	:APlayerController()
{
	bReplicates = true;
}

void ASTPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	CheatComponent = Cast<USTCheatComponent>(GetComponentByClass(CheatComponentClass));
}

bool ASTPlayerControllerBase::ProcessConsoleExec(const TCHAR* Str, FOutputDevice& Ar, UObject* Executor)
{
	if (CheatComponent)
	{
		CheatComponent->ProcessConsoleExec(Str, Ar, Executor);;
	}		
	return Super::ProcessConsoleExec(Str,Ar,Executor);
}


ASTPlayerControllerBase_StandAlone::ASTPlayerControllerBase_StandAlone()
	:ASTPlayerControllerBase()
{
}


ASTPlayerController_ServerBase::ASTPlayerController_ServerBase()
	:ASTPlayerControllerBase()
{

}

void ASTPlayerController_ServerBase::ConfirmAccountData()
{
	ConfirmAccountData_Client();
}

void ASTPlayerController_ServerBase::ConfirmAccountData_Client_Implementation()
{
	ConfirmAccountData_Server(UAccountManager::GetAccountManager()->GetLocalAccountData());
}

void ASTPlayerController_ServerBase::ConfirmAccountData_Server_Implementation(const FAccountData& AccountData)
{
	PlayerAccountData = MakeShared<FAccountData>(AccountData);	
	ISTGameSessionInterface::Execute_ProcessSpawning(UGameplayStatics::GetGameMode(this)->GameSession, this, AccountData);
}
