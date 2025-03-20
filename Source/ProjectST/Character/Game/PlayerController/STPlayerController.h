// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "STNetPlayerControllerInterface.h"
#include "STNet/Public/Generated/GeneratedStructs.h"
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

	UFUNCTION(BlueprintCallable)
	void Dummy() {}
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

	virtual void ConfirmAccountData() override;

	virtual TSharedPtr<FAccountData> GetAccountData()override { return PlayerAccountData; }

	UFUNCTION(Client,Reliable)
	void ConfirmAccountData_Client();

	UFUNCTION(Server, Reliable)
	void ConfirmAccountData_Server(const FAccountData& AccountData);

private:

	TSharedPtr<FAccountData> PlayerAccountData;

};
