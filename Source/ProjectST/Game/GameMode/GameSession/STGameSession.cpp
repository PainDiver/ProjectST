// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameMode/GameSession/STGameSession.h"
#include "Character/Game/PlayerController/STNetPlayerControllerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/GlobalMacros.h"
#include "STNet/Public/Generated/GeneratedStructs.h"
#include "Misc/STEventManager.h"
#include "Character/Game/PlayerController/STPlayerController.h"

ASTGameSessionBase::ASTGameSessionBase()
	:AGameSession()
{
}

void ASTGameSessionBase::BeginDestroy()
{
	AGameSession::BeginDestroy();

}

void ASTGameSessionBase::PostLogin(APlayerController* NewPlayer)
{
	AGameSession::PostLogin(NewPlayer);

	if (ISTNetPlayerControllerInterface* IPC = Cast<ISTNetPlayerControllerInterface>(NewPlayer))
	{
		IPC->ConfirmAccountData();
	}
}

void ASTGameSessionBase::NotifyLogout(const APlayerController* Controller)
{
	if (Controller == nullptr)
	{
		return;
	}

	if (ASTPlayerControllerBase* STController = const_cast<ASTPlayerControllerBase*>(Cast<ASTPlayerControllerBase>(Controller)))
	{
		STController->OnLogout();
	}

	OnLogout(Controller);

	for (TPair<FString, APlayerController*> Player : ConnectedPlayers)
	{
		if (Player.Value == Controller)
		{
			ConnectedPlayers.Remove(Player.Key);
			break;
		}
	}

	AGameSession::NotifyLogout(Controller);
}
	
bool ASTGameSessionBase::ProcessSpawning_Implementation(APlayerController* PlayerController, const FAccountData& AccountData)
{
	FAccountData SpawnAccountParam;
#if WITH_DB_SERVER
	TPair<int64, FString> ValidationData{ AccountData.UserUID,AccountData.LoginToken };
	if (FAccountData* ConnectingPlayer = ConnectingPlayers.Find(ValidationData))
	{
		SpawnAccountParam = *ConnectingPlayer;
		ConnectingPlayers.Remove(ValidationData);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Valid User Trying to connect"));
		return false;
	}
#else
	SpawnAccountParam = AccountData;
#endif	

	if (ConnectedPlayers.Contains(AccountData.ID))
	{
		ConnectedPlayers.Add(AccountData.ID+"_", PlayerController);
	}
	else
	{
		ConnectedPlayers.Add(AccountData.ID, PlayerController);
	}

	FTransform OutTransform;
	APawn* Spawned = SpawnPlayer(PlayerController, SpawnAccountParam, OutTransform);
	if (Spawned == nullptr)
	{
		return false;
	}

	PlayerController->SetPawn(Spawned);
	PlayerController->Possess(Spawned);
	PlayerController->ClientSetRotation(Spawned->GetActorRotation(), true);
	Spawned->SetActorLocation(OutTransform.GetLocation());

	FRotator NewControllerRot = OutTransform.GetRotation().Rotator();
	NewControllerRot.Roll = 0.f;
	PlayerController->SetControlRotation(NewControllerRot);
	Spawned->SetPlayerDefaults();

	if (AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld()))
	{		
		GameMode->K2_OnRestartPlayer(PlayerController);
	}

	return true;
}
