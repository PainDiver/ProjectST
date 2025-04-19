// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerState.h"
#include "Misc/STEnum.h"
#include "Game/STGameState.h"
#include "Data/DataTableManager.h"
#include "GameFramework/PlayerState.h"
#include "Misc/STEventManager.h"

// Sets default values for this component's properties
USTInventoryComponent::USTInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USTInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...	
	
	if (GetOwner()->HasAuthority())
	{
		InventoryItemDatas.Initialize(InventoryContainerSize);
		EquippedItemDatas.Initialize((int32)EEquipSlotType::MAX);
	}
}


// Called every frame
void USTInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USTInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(USTInventoryComponent, InventoryItemDatas, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(USTInventoryComponent, EquippedItemDatas, Params);
}

//void USTInventoryComponent::RegisterInventorySearcher(UObject* PS)
//{
//	InventoryItemDatas.RegisterInventorySearcher(PS);
//}
//
//void USTInventoryComponent::UnregisterInventorySearcher(UObject* PS)
//{
//	InventoryItemDatas.UnregisterInventorySearcher(PS);
//}

void USTInventoryComponent::RequestDragAndDrop_Server_Implementation(
	USTInventoryComponent* SourceInventory,
	EItemContainerType SourceInventoryType,
	int32 SourceIndex, 
	USTInventoryComponent* TargetInventory, 
	EItemContainerType TargetInventoryType, 
	int32 TargetIndex)
{
	DragAndDropItem(SourceInventory,SourceInventoryType, SourceIndex, TargetInventory, TargetInventoryType, TargetIndex);
}

bool USTInventoryComponent::CanDoDragAndDropOperation(
	EInventoryOperationType OperationType, 
	EItemContainerType SourceInventoryType, 
	const FItemInfoData& SourceItem, 
	EItemContainerType TargetInventoryType, 
	const FItemInfoData& TargetItem,
	EEquipSlotType TargetEquipSlotType
)
{
	UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
	if (TableManager == nullptr)
		return false;

	if (TargetInventoryType == EItemContainerType::EQUIPMENT||
		(OperationType == EInventoryOperationType::SWAP && TargetInventoryType == EItemContainerType::INVENTORY))
	{
		switch (OperationType)
		{
			case EInventoryOperationType::MOVE:
			{
				FItemEquipInfoData SourceEquipData;
				if (!TableManager->GetTableData(TableManager->EquipItemDataTable, SourceItem.EquipInfoID, SourceEquipData))
				{
					return false;
				}

				return SourceEquipData.ItemSlotType == TargetEquipSlotType;
			}
			case EInventoryOperationType::STACK:
			{
				return false;
			}
			case EInventoryOperationType::SWAP:
			{
				FItemEquipInfoData SourceEquipData;
				TableManager->GetTableData(TableManager->EquipItemDataTable, SourceItem.EquipInfoID, SourceEquipData);

				FItemEquipInfoData TargetEquipData;
				TableManager->GetTableData(TableManager->EquipItemDataTable, TargetItem.EquipInfoID, TargetEquipData);

				return SourceEquipData.ItemSlotType == TargetEquipData.ItemSlotType;
			}
		}
	}

	return true;
}

void USTInventoryComponent::DragAndDropItem(
	USTInventoryComponent* SourceInventory,
	EItemContainerType SourceInventoryType,
	int32 SourceIndex,
	USTInventoryComponent* TargetInventory,
	EItemContainerType TargetInventoryType,
	int32 TargetIndex)
{
	if (SourceInventory == nullptr || TargetInventory == nullptr)
		return;

	FReplicatedItemData SourceItemData;
	bool bIsSourceItemValid = SourceInventory->GetItemAt(SourceInventoryType, SourceIndex, SourceItemData);
	if (!bIsSourceItemValid)
		return;

	FReplicatedItemData TargetItemData;
	bool bIsTargetItemValid = TargetInventory->GetItemAt(TargetInventoryType, TargetIndex, TargetItemData);
	bool bIsSameContainerType = (SourceInventoryType == TargetInventoryType);

	EInventoryOperationType OperationType = EInventoryOperationType::NONE;
	// 아이템 동일 -> Stack Operation
	if (bIsSourceItemValid &&
		bIsTargetItemValid &&
		SourceItemData.ItemID == TargetItemData.ItemID)
	{
		OperationType = EInventoryOperationType::STACK;
	}	
	// Source와 Target 모두 존재 -> Swap Operation
	else if (bIsSourceItemValid &&
		bIsTargetItemValid)
	{
		OperationType = EInventoryOperationType::SWAP;
	}
	// Source만 존재 -> Move
	else if(bIsSourceItemValid)
	{
		OperationType = EInventoryOperationType::MOVE;
	}

	if (!CanDoDragAndDropOperation(OperationType,SourceInventoryType, SourceItemData.ItemInfo, TargetInventoryType, TargetItemData.ItemInfo,(EEquipSlotType)TargetIndex))
	{
		return;
	}


	switch (OperationType)
	{
	case EInventoryOperationType::MOVE:
	{
		FReplicatedItemData OutRemoved;
		SourceInventory->GetContainer(SourceInventoryType)->RemoveItem(SourceItemData, SourceItemData.ItemCount, OutRemoved);
		TargetInventory->GetContainer(TargetInventoryType)->AddItemAt(TargetIndex, OutRemoved);
		break;
	}
	case EInventoryOperationType::STACK:
	{		
		int32 RemainingCount = TargetItemData.ItemInfo.MaxCount - TargetItemData.ItemCount;
		FReplicatedItemData OutRemoved;
		SourceInventory->GetContainer(SourceInventoryType)->RemoveItem(SourceIndex, RemainingCount, OutRemoved);
		TargetInventory->GetContainer(TargetInventoryType)->AddItemCount(TargetIndex, RemainingCount);		
		break;
	}
	case EInventoryOperationType::SWAP:
	{
		SourceInventory->GetContainer(SourceInventoryType)->ModifyItem(SourceIndex, TargetItemData);
		TargetInventory->GetContainer(TargetInventoryType)->ModifyItem(TargetIndex, SourceItemData);		
		break;
	}
	default:
		break;

	}
}


void USTInventoryComponent::AcquireItem(const FReplicatedItemData& Item)
{
	InventoryItemDatas.AddItem(Item);
}


//void USTInventoryComponent::DropItem(const FReplicatedItemData& Item)
//{
//	if (ASTGameState* GameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
//	{
//		//Set Drop Location or Drop 
//		if (APawn* Pawn = Cast<APawn>(GetOwner()))
//		{
//			GameState->RealizeItemActor(Item, Pawn->GetActorLocation());
//		}
//	}
//}

bool USTInventoryComponent::GetItemAt(EItemContainerType ContainerType, int32 Index, FReplicatedItemData& OutItem)
{
	switch (ContainerType)
	{
	case EItemContainerType::EQUIPMENT:
	{
		return EquippedItemDatas.FindItem(Index, OutItem);
		break;
	}
	case EItemContainerType::INVENTORY:
	{
		return InventoryItemDatas.FindItem(Index, OutItem);
		break;
	}
	default:
		return false;
	}
}

FReplicatedItemContainer* USTInventoryComponent::GetContainer(EItemContainerType ContainerType)
{
	switch (ContainerType)
	{
	case EItemContainerType::EQUIPMENT:
		return &EquippedItemDatas;
	case EItemContainerType::INVENTORY:
		return &InventoryItemDatas;
	default:
		return nullptr;
	}
}

void USTInventoryComponent::BindOnAddItem(EItemContainerType ContainerType, FOnAddItem_Elem Delegate)
{
	GetContainer(ContainerType)->GetOnAddItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index,const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index, Data);
		}
	);
}

void USTInventoryComponent::BindOnRemoveItem(EItemContainerType ContainerType, FOnRemoveItem_Elem Delegate)
{
	GetContainer(ContainerType)->GetOnRemoveItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index, const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index, Data);
		}
	);

}

void USTInventoryComponent::BindOnModifyItem(EItemContainerType ContainerType, FOnModifyItem_Elem Delegate)
{
	GetContainer(ContainerType)->GetOnModifyItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index, const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index,Data);
		}
	);
}

void USTInventoryComponent::RegisterEquipmentActor(ASTEquipItemActor* EquipActor)
{
	EquippedItemDatas.RegisterEquipmentActor(EquipActor);
}

void USTInventoryComponent::UnregisterEquipmentActor(const FReplicatedItemData& RemovedItemData)
{
	EquippedItemDatas.UnregisterEquipmentActor(RemovedItemData);
}
