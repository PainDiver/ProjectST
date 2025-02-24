// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Game/Item/ReplicatedItemData.h"
#include "Game/Item/STItemContainerInterface.h"
#include "STGameState.generated.h"

/**
 * 
 */
class USTItemObject;
class ASTItemActor;

namespace ItemTracing
{
	enum ContainerType
	{
		Item,
		Container
	};
	enum ItemState
	{
		Alive,
		Dead
	};
}

USTRUCT()
struct FItemTracingInfo
{
	GENERATED_BODY()


	void Initialize(ItemTracing::ItemState State ,ItemTracing::ContainerType NewType, FReplicatedItemContainer* NewContainerPtr, FGuid NewItemContainerUID)
	{
		ItemState = State;
		ContainerType = NewType;
		ContainerPtr = NewContainerPtr;
		ItemContainerUID = NewItemContainerUID;
		RecordTime = FDateTime::Now();
	}

	void Remove()
	{
		ItemState = ItemTracing::ItemState::Dead;
		ContainerPtr = nullptr;
		ItemContainerUID = FGuid();
		RecordTime = FDateTime::Now();
	}

	ItemTracing::ItemState ItemState;
	ItemTracing::ContainerType ContainerType;
	FReplicatedItemContainer* ContainerPtr; //컨테이너의 경우 복사안되므로
	FGuid ItemContainerUID; // 아이템컨테이너의 경우 컨테이너가 되는 아이템
	FDateTime RecordTime;
};

UCLASS()
class PROJECTST_API ASTGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ASTGameState(const FObjectInitializer& OI);
	~ASTGameState();

	// Owner없이 ItemData만 생성
	UFUNCTION(BlueprintCallable)
	bool CreateItemObject(FReplicatedItemData& OutData, int32 ItemID, int32 Count);
	
	// Owner(Container)내부에 ItemData 생성
	UFUNCTION(BlueprintCallable)
	bool CreateItemObjectToOwner(FReplicatedItemData& OutData, int32 ItemID, int32 Count, UObject* OwnerContainer,EItemContainerType ContainerType);
	
	// Owner(아이템을 컨테이너로 해서(주머니같은))내부에 ItemData 생성
	UFUNCTION(BlueprintCallable)
	bool CreateItemObjectToItem(FReplicatedItemData& OutData, int32 ItemID, int32 Count, UPARAM(Ref) FReplicatedItemData& TargetItem);

	// Owner(아이템을 컨테이너로 해서(주머니같은))내부에 ItemData 생성
	UFUNCTION(BlueprintCallable)
	bool CreateItemObjectToItemFromOwner(FReplicatedItemData& OutData, int32 ItemID, int32 Count, UPARAM(Ref) FReplicatedItemData& TargetItem, UObject* OwnerContainer, EItemContainerType ContainerType);
	bool CreateItemObject_Internal(FReplicatedItemData& OutData,int32 ItemID, int32 Count, FReplicatedItemContainer* ItemContainerPtr);


	UFUNCTION(BlueprintCallable)
	ASTItemActor* CreateItemActor(int32 ItemID,const FVector& Loc,const FRotator Rot = FRotator::ZeroRotator,int32 Count = 1);
	ASTItemActor* RealizeItemActor(const FReplicatedItemData& ItemData, const FVector& Loc, const FRotator Rot = FRotator::ZeroRotator);


	static void RecordItemTrackingInfo(FGuid ItemUID,ItemTracing::ContainerType ContainerType,FReplicatedItemContainer* ContainerPtr, FGuid ItemContainerUID);
	static void RemoveItemTrackingInfo(FGuid ItemUID);

private:

	// 아이템 추적용, 만약 DB가 붙으면 이 정보를 가지고 
	// 데디서버게임이 끝나면 데이터 올리기
	static TMap<FGuid, FItemTracingInfo> ItemTracker;
};
