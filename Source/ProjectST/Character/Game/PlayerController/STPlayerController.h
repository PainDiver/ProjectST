// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STNetPlayerControllerInterface.h"
#include "Data/DataStructs/Data_AccountData.h"
#include "STPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API ASTPlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
public:
	ASTPlayerControllerBase();

};

UCLASS()
class PROJECTST_API ASTPlayerControllerBase_StandAlone : public ASTPlayerControllerBase
{
	GENERATED_BODY()

public:
	ASTPlayerControllerBase_StandAlone();
};

UCLASS()
class PROJECTST_API ASTPlayerController_ServerBase : public ASTPlayerControllerBase, public ISTNetPlayerControllerInterface
{
	GENERATED_BODY()
	
public:	
	ASTPlayerController_ServerBase();

	virtual void ProcessSpawning_Implementation()override;

	UFUNCTION(Reliable,Client)
	void ConfirmSpawning_Client();
	void ConfirmSpawning_Client_Implementation();

	UFUNCTION(Reliable, Server)
	void ProcessSpawning_Server(const FAccountData& AccountData);
	void ProcessSpawning_Server_Implementation(const FAccountData& AccountData);


	// Lobby없이 실행 시
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FAccountData DummyAccountData;

};
