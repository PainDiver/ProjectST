// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameState.h"
#include "Game/Item/STItemActor.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

TMap<FGuid, FItemTracingInfo> ASTGameState::ItemTracker = TMap<FGuid, FItemTracingInfo>();

ASTGameState::ASTGameState(const FObjectInitializer& OI)
	:Super(OI)
{
}

ASTGameState::~ASTGameState()
{
	ItemTracker.Empty();
}

bool ASTGameState::CreateItemObject(FReplicatedItemData& OutData, int32 ItemID, int32 Count)
{
	return CreateItemObject_Internal(OutData, ItemID, Count,nullptr);
}

bool ASTGameState::CreateItemObjectToOwner(FReplicatedItemData& OutData, int32 ItemID, int32 Count, UObject* OwnerContainer, EItemContainerType ContainerType)
{
	if (OwnerContainer == nullptr)
		return false;

	if (ISTItemContainerInterface* Interface = Cast<ISTItemContainerInterface>(OwnerContainer))
	{
		FReplicatedItemContainer* ItemContainerPtr = Interface->GetContainer(ContainerType);
		if (ItemContainerPtr)
		{
			return CreateItemObject_Internal(OutData, ItemID, Count, ItemContainerPtr);
		}
	}
	return false;
}

bool ASTGameState::CreateItemObjectToItem(FReplicatedItemData& OutData, int32 ItemID, int32 Count, FReplicatedItemData& TargetItem)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Created for 0 Count"));
		return false;
	}

	FReplicatedItemData NewReplicatedData;
	NewReplicatedData.Initialize(this, ItemID, Count);	

	RecordItemTrackingInfo(NewReplicatedData.ItemUID, ItemTracing::ContainerType::Item, nullptr, TargetItem.ItemUID);

	TargetItem.AddAsContainer(nullptr,NewReplicatedData);
	OutData = NewReplicatedData;

	return true;
}

bool ASTGameState::CreateItemObjectToItemFromOwner(FReplicatedItemData& OutData, int32 ItemID, int32 Count, FReplicatedItemData& TargetItem, UObject* OwnerContainer, EItemContainerType ContainerType)
{
	if (OwnerContainer == nullptr)
		return false;

	if (ISTItemContainerInterface* Interface = Cast<ISTItemContainerInterface>(OwnerContainer))
	{
		if (FReplicatedItemContainer* ItemContainerPtr = Interface->GetContainer(ContainerType))
		{
			FReplicatedItemData NewReplicatedData;
			NewReplicatedData.Initialize(this, ItemID, Count);
			
			RecordItemTrackingInfo(NewReplicatedData.ItemUID, ItemTracing::ContainerType::Item, ItemContainerPtr, TargetItem.ItemUID);

			TargetItem.AddAsContainer(ItemContainerPtr, NewReplicatedData);
			OutData = NewReplicatedData;
		}
	}

	return false;
}

bool ASTGameState::CreateItemObject_Internal(FReplicatedItemData& OutData, int32 ItemID,int32 Count, FReplicatedItemContainer* ItemContainerPtr)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Created for 0 Count"));
		return false;
	}

	FReplicatedItemData NewReplicatedData;	
	NewReplicatedData.Initialize(this, ItemID, Count);
	if (ItemContainerPtr)
	{
		ItemContainerPtr->AddItem(NewReplicatedData);
	}

	RecordItemTrackingInfo(NewReplicatedData.ItemUID, ItemTracing::ContainerType::Container, ItemContainerPtr, FGuid());

	OutData = NewReplicatedData;

	return true;
}


ASTItemActor* ASTGameState::RealizeItemActor(const FReplicatedItemData& ItemData, const FVector& Loc, const FRotator Rot)
{	
	FActorSpawnParameters Params;
	Params.Owner = this;
	ASTItemActor* NewItemActor = GetWorld()->SpawnActor<ASTItemActor>(ASTItemActor::StaticClass(), Loc, Rot, Params);
	NewItemActor->Initialize(ItemData);

	return NewItemActor;
}

void ASTGameState::RecordItemTrackingInfo(FGuid ItemUID, ItemTracing::ContainerType ContainerType, FReplicatedItemContainer* ContainerPtr, FGuid ItemContainerUID)
{
	FItemTracingInfo TracingInfo;
	TracingInfo.Initialize(ItemTracing::ItemState::Alive,ContainerType, ContainerPtr, ItemContainerUID);
	ItemTracker.Add(ItemUID, TracingInfo);
}

void ASTGameState::RemoveItemTrackingInfo(FGuid ItemUID)
{
	if (FItemTracingInfo* Info = ItemTracker.Find(ItemUID))
	{
		Info->Remove();
	}
}


ASTItemActor* ASTGameState::CreateItemActor(int32 ItemID, const FVector& Loc, const FRotator Rot, int32 Count)
{
	if (Count <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Item Created for 0 Count"));
		return nullptr;
	}

	FReplicatedItemData ItemData;
	if (CreateItemObject(ItemData, ItemID, Count))
	{
		return RealizeItemActor(ItemData, Loc, Rot);
	}
	return nullptr;
}

