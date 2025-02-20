// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Game/STGameState.h"
#include "Game/Item/STItemActor.h"
#include "Game/Item/STItemObject.h"
#include "Net/UnrealNetwork.h"


USTItemObject* ASTGameState::CreateItemObject(int32 ItemID,int32 Count)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Created for 0 Count"));
		return nullptr;
	}
	USTItemObject* NewItem = NewObject<USTItemObject>(this);
	NewItem->Initialize(ItemID);

	FReplicatedItemData NewReplicatedData;
	NewReplicatedData.ItemUID = FGuid::NewGuid();
	NewReplicatedData.Count = Count;
	NewReplicatedData.Owner = this;

	ReplicatedItemDatas.AddItem(NewReplicatedData);

	return NewItem;
}

ASTItemActor* ASTGameState::RealizeItemActor(USTItemObject* ItemBase, const FVector& Loc, const FRotator Rot)
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	ASTItemActor* NewItemActor = GetWorld()->SpawnActor<ASTItemActor>(ASTItemActor::StaticClass(), Loc, Rot, Params);
	NewItemActor->Initialize(ItemBase);

	return NewItemActor;
}


ASTItemActor* ASTGameState::CreateItemActor(int32 ItemID, const FVector& Loc, const FRotator Rot, int32 Count)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Created for 0 Count"));
		return nullptr;
	}

	USTItemObject* NewItemObject = CreateItemObject(ItemID, Count);	
	return RealizeItemActor(NewItemObject,Loc,Rot);
}

void ASTGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(ASTGameState,ReplicatedItemDatas,Params);
}
