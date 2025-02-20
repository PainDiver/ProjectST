// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Game/Item/ReplicatedItemData.h"
#include "STGameState.generated.h"

/**
 * 
 */
class USTItemObject;
class ASTItemActor;

UCLASS()
class PROJECTST_API ASTGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	USTItemObject* CreateItemObject(int32 ItemID, int32 Count =1);

	UFUNCTION(BlueprintCallable)
	ASTItemActor* RealizeItemActor(USTItemObject* ItemBase, const FVector& Loc, const FRotator Rot = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable)
	ASTItemActor* CreateItemActor(int32 ItemID,const FVector& Loc,const FRotator Rot = FRotator::ZeroRotator,int32 Count = 1);


private:

	UPROPERTY(Replicated)
	FReplicatedItemContainer ReplicatedItemDatas;
};
