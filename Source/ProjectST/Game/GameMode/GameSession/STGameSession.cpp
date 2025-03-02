// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameMode/GameSession/STGameSession.h"
#include "Character/Game/PlayerController/STNetPlayerControllerInterface.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/GlobalMacros.h"

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

	// 만약 DB서버있으면..
	// 1. 유저 접속 시 DB에게 UserAccount정보와, 토큰발행을 요청하고 클라이언트에 부여 (클라는 AccountSystem에서 저장)
	// 2. 유저의 매칭 -> DB서버에게 토큰으로 매칭요청
	// 3. DB서버의 매칭 -> 데디케이트서버에게 클라 토큰으로 UserInfo전달
	// 4. 데디서버가 토큰과,UserInfo 받고 저장한다음 DB서버에게 다시 알려줌
	// 5. DB서버는 매칭중인 클라에게 데디케이트서버 접속방법 제시
	// 6. OnPostLogin에서 클라가 데디서버에게도 토큰전달(RPC)
	// 7. 데디는 RPC받고 토큰 매칭되면 스폰, 매칭안되면 스폰X 추방
	
	// 현재 DB서버없이 포트폴리오 만드는중이니까 Json으로 UserAccount를 만들어
	// DB의 역할을 하도록 함, 클라이언트의 정보를 그대로 신뢰하도록함
	// 1. 유저 접속 시, Json에서 UserAccount 부여 (클라는 AccountSystem에서 저장)
	// 2. OnPostLogin에서 클라가 데디서버에게 UserInfo 전달 (여기서는 ID)
	// 3. UserInfo를 통해서 캐릭터 스폰	
	
	ISTNetPlayerControllerInterface::Execute_ProcessSpawning(NewPlayer); // Client RPC
}

void ASTGameSessionBase::NotifyLogout(const APlayerController* Controller)
{
	AGameSession::NotifyLogout(Controller);

	if (Controller == nullptr)
	{
		return;
	}
	
	for (TPair<int64, APlayerController*> Player : ConnectedPlayers)
	{
		if (Player.Value == Controller)
		{
			ConnectedPlayers.Remove(Player.Key);
			return;
		}
	}	
}
	
bool ASTGameSessionBase::FinishSpawning_Implementation(APlayerController* PlayerController, const FAccountData& AccountData)
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

	ConnectedPlayers.Add(AccountData.UserUID,PlayerController);

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
