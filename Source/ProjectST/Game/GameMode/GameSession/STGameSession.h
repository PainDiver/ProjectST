// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"
#include "Data/DataStructs/Data_AccountData.h"
#include "STGameSessionInterface.h"
#include "STGameSession.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTGameSessionBase : public AGameSession,public ISTGameSessionInterface
{
	GENERATED_BODY()
	
public:
	ASTGameSessionBase();

	virtual void BeginDestroy() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void NotifyLogout(const APlayerController* PC)override;
	
	bool FinishSpawning_Implementation(APlayerController* PlayerController, const FAccountData& AccountData);
	
	UFUNCTION(BlueprintImplementableEvent)
	APawn* SpawnPlayer(APlayerController* PlayerController, const FAccountData& AccountData,FTransform& OutTransform);
	
private:
	
	//User UID,Controller
	UPROPERTY()
	TMap<int64, APlayerController*> ConnectedPlayers;
	
	TMap<TPair<int64, FString>, FAccountData> ConnectingPlayers;

};
