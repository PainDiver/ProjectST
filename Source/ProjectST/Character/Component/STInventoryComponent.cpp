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
	InventoryItemDatas.SetContainerSize(InventoryContainerSize,InventoryMaxColCount);
	EquippedItemDatas.SetContainerSize((int32)EEquipSlotType::MAX, 1);
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

	DOREPLIFETIME(USTInventoryComponent, InventoryContainerSize);
	DOREPLIFETIME(USTInventoryComponent, InventoryMaxColCount);
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
	SourceInventory->GetItemAt(SourceInventoryType, SourceIndex, SourceItemData);
	
	FReplicatedItemData TargetItemData;
	TargetInventory->GetItemAt(TargetInventoryType, TargetIndex, TargetItemData);

	FReplicatedItemContainer* SourceContainer = SourceInventory->GetContainer(SourceInventoryType);
	FReplicatedItemContainer* TargetContainer = TargetInventory->GetContainer(TargetInventoryType);

	SourceContainer->FindItem(SourceItemData.ParentIndex, SourceItemData);
	TargetContainer->FindItem(TargetItemData.ParentIndex, TargetItemData);

	EInventoryOperationType OperationType = SelectOperation(
		SourceItemData,
		SourceContainer,
		SourceIndex,
		TargetItemData,
		TargetContainer,
		TargetIndex	);

	if (OperationType == EInventoryOperationType::NONE)
	{
		return;
	}

	switch (OperationType)
	{
	case EInventoryOperationType::MOVE:
	{
		FReplicatedItemData OutRemoved;
		SourceContainer->RemoveItemByItem(SourceItemData, SourceItemData.ItemCount, OutRemoved);
		TargetContainer->AddItemAt(TargetIndex, OutRemoved);
		break;
	}
	case EInventoryOperationType::STACK:
	{		
		int32 RemainingCount = TargetItemData.ItemInfo.MaxCount - TargetItemData.ItemCount;
		int32 SourceItemCount = SourceItemData.ItemCount;
		int32 RemoveCount = FMath::Clamp(SourceItemCount, 0, RemainingCount);		

		FReplicatedItemData OutRemoved;
		SourceContainer->RemoveItemAt(SourceIndex, RemoveCount, OutRemoved);
		TargetContainer->AddItemCount(TargetIndex, RemoveCount);
		break;
	}
	case EInventoryOperationType::SWAP:
	{
		SourceContainer->ModifyItemAt(SourceIndex, TargetItemData);
		TargetContainer->ModifyItemAt(TargetIndex, SourceItemData);
		break;
	}
	default:
		break;

	}
}

bool USTInventoryComponent::CanStack(const FReplicatedItemData& SourceItemData, const FReplicatedItemData& TargetItemData)
{
	if (SourceItemData.ItemInfo.ID == TargetItemData.ItemInfo.ID)
	{
		if (SourceItemData.ItemInfo.EquipInfoID <= 0 && TargetItemData.ItemInfo.EquipInfoID <= 0)
		{
			return TargetItemData.ItemCount < TargetItemData.ItemInfo.MaxCount;
		}
	}
	return false;
}

EInventoryOperationType USTInventoryComponent::SelectOperation(
	const FReplicatedItemData& SourceItemData,
	FReplicatedItemContainer* SourceContainer,
	int32 SourceIndex,
	const FReplicatedItemData& TargetItemData,
	FReplicatedItemContainer* TargetContainer,
	int32 TargetIndex)
{
	UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
	if (TableManager == nullptr)
		return EInventoryOperationType::NONE;

	bool bIsSourceItemValid = SourceItemData.IsValid();
	bool bIsTargetItemValid = TargetItemData.IsValid();
	
	EItemContainerType TargetInventoryType = TargetContainer->GetContainerType();
	EItemContainerType SourceInventoryType = SourceContainer->GetContainerType();

	FReplicatedItemData TargetParentItem;
	TargetContainer->FindItem(TargetItemData.ParentIndex, TargetParentItem);

	FReplicatedItemData SourceParentItem;
	SourceContainer->FindItem(SourceItemData.ParentIndex, SourceParentItem);

	if (TargetInventoryType == EItemContainerType::EQUIPMENT)
	{
		if (SourceParentItem.ItemInfo.EquipInfoID <= 0)
		{
			return EInventoryOperationType::NONE;
		}

		FItemEquipInfoData SourceEquipData;
		TableManager->GetTableData(TableManager->EquipItemDataTable, SourceParentItem.ItemInfo.EquipInfoID, SourceEquipData);
		if (SourceEquipData.ItemSlotType != (EEquipSlotType)TargetIndex)
		{
			return EInventoryOperationType::NONE;
		}
	}

	TArray<int32> Neighbors;
	if (TargetContainer->CheckSpaceForInventory(TargetIndex, SourceParentItem.ItemInfo.ItemSize, SourceParentItem,Neighbors))
	{
		return EInventoryOperationType::MOVE;
	}
	else if(bIsSourceItemValid && bIsTargetItemValid)
	{
		if (TargetParentItem.ItemID == SourceParentItem.ItemID &&
			TargetParentItem.ItemCount < TargetParentItem.ItemInfo.MaxCount &&
			TargetParentItem.ItemInfo.EquipInfoID <= 0
			)
		{
			return EInventoryOperationType::STACK;
		}
		else if(SourceParentItem.ItemInfo.ItemSize == TargetParentItem.ItemInfo.ItemSize)
		{
			if (SourceParentItem.ItemInfo.EquipInfoID == 0 && SourceParentItem.ItemInfo.EquipInfoID == 0)
			{
				return EInventoryOperationType::SWAP;
			}

			FItemEquipInfoData SourceEquipData;
			TableManager->GetTableData(TableManager->EquipItemDataTable, SourceParentItem.ItemInfo.EquipInfoID, SourceEquipData);
			FItemEquipInfoData TargetEquipData;
			TableManager->GetTableData(TableManager->EquipItemDataTable, TargetParentItem.ItemInfo.EquipInfoID, TargetEquipData);
			if (TargetEquipData.ItemSlotType == SourceEquipData.ItemSlotType)
			{
				return EInventoryOperationType::SWAP;
			}		
		}
	}

	return EInventoryOperationType::NONE;
}

void USTInventoryComponent::RequestSplit_Implementation
(
	USTInventoryComponent* SourceInventory,
	EItemContainerType SourceInventoryType,
	int32 SourceIndex,
	USTInventoryComponent* TargetInventory,
	EItemContainerType TargetInventoryType,
	int32 TargetIndex,
	int32 CountToSplit
)
{
	FReplicatedItemData SourceItemData;
	SourceInventory->GetItemAt(SourceInventoryType, SourceIndex, SourceItemData);

	FReplicatedItemData TargetItemData;
	TargetInventory->GetItemAt(TargetInventoryType, TargetIndex, TargetItemData);

	if (TargetItemData.IsValid() && SourceItemData.ItemID != TargetItemData.ItemID)
	{
		return;
	}


	FReplicatedItemContainer* SourceContainer = SourceInventory->GetContainer(SourceInventoryType);
	FReplicatedItemContainer* TargetContainer = TargetInventory->GetContainer(TargetInventoryType);

	if (SourceItemData.ItemCount >= CountToSplit)
	{
		FReplicatedItemData OutRemoved;
		SourceContainer->RemoveItemAt(SourceIndex, CountToSplit, OutRemoved);
		
		if (TargetItemData.IsValid())
		{
			TargetContainer->AddItemCount(TargetIndex, CountToSplit);
		}
		else
		{
			FReplicatedItemData NewItem;
			NewItem.Initialize(this, SourceItemData.ItemID, CountToSplit);
			TargetContainer->AddItemAt(TargetIndex,NewItem);
		}
	}
}

void USTInventoryComponent::AcquireItem(const FReplicatedItemData& Item)
{
	InventoryItemDatas.AddItemAny(Item);
}

void USTInventoryComponent::RequestDropItem_Server_Implementation(EItemContainerType ContainerType, const FGuid& ItemUID)
{
	FReplicatedItemData ItemFound;
	if (GetContainer(ContainerType)->FindItem(ItemUID, ItemFound))
	{
		DropItem(ContainerType,ItemFound);
	}
}

bool USTInventoryComponent::DropItem(EItemContainerType ContainerType, const FReplicatedItemData& Item)
{
	if (ASTGameState* GameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		FReplicatedItemData ItemFound;
		if (GetContainer(ContainerType)->FindItem(Item.ItemUID, ItemFound))
		{
			FReplicatedItemData ItemData;
			if (GetContainer(ContainerType)->RemoveItemByItem(Item, Item.ItemCount, ItemData))
			{				
				if (GameState->RealizeItemActor(Item, GetOwnerActor()->GetActorLocation() + FVector(0, 0, 40)))
				{
					return true;
				}
			}
		}
	}

	return false;
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

bool USTInventoryComponent::CheckSpaceForInventory(int32 Index, const FIntPoint& Size,const FReplicatedItemData& IgnoredItem ,TArray<int32>& OutNeighbors)
{
	return InventoryItemDatas.CheckSpaceForInventory(Index,Size, IgnoredItem,OutNeighbors);
}

AActor* USTInventoryComponent::GetOwnerActor() const
{
	if (APlayerState* PS = Cast<APlayerState>(GetOwner()))
	{
		return PS->GetPawn();
	}
	return GetOwner();
}

FBlueprintExposedDelegateHandle USTInventoryComponent::BindOnAddItem(EItemContainerType ContainerType, FOnAddItem_Elem Delegate)
{
	FDelegateHandle Handle= GetContainer(ContainerType)->GetOnAddItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index,const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index, Data);
		}
	);

	return FBlueprintExposedDelegateHandle(Handle);
}

FBlueprintExposedDelegateHandle USTInventoryComponent::BindOnRemoveItem(EItemContainerType ContainerType, FOnRemoveItem_Elem Delegate)
{
	FDelegateHandle Handle = GetContainer(ContainerType)->GetOnRemoveItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index, const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index, Data);
		}
	);

	return FBlueprintExposedDelegateHandle(Handle);
}

FBlueprintExposedDelegateHandle USTInventoryComponent::BindOnModifyItem(EItemContainerType ContainerType, FOnModifyItem_Elem Delegate)
{
	FDelegateHandle Handle = GetContainer(ContainerType)->GetOnModifyItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](int32 Index, const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Index,Data);
		}
	);
	
	return FBlueprintExposedDelegateHandle(Handle);
}

void USTInventoryComponent::ClearOnAddItem(EItemContainerType ContainerType, const FBlueprintExposedDelegateHandle& DelegateHandle)
{
	GetContainer(ContainerType)->GetOnAddItem().Remove(DelegateHandle.Handle);
}

void USTInventoryComponent::ClearOnRemoveItem(EItemContainerType ContainerType, const FBlueprintExposedDelegateHandle& DelegateHandle)
{
	GetContainer(ContainerType)->GetOnRemoveItem().Remove(DelegateHandle.Handle);
}

void USTInventoryComponent::ClearOnModifyItem(EItemContainerType ContainerType, const FBlueprintExposedDelegateHandle& DelegateHandle)
{
	GetContainer(ContainerType)->GetOnModifyItem().Remove(DelegateHandle.Handle);
}

void USTInventoryComponent::RegisterEquipmentActor(ASTEquipItemActor* EquipActor)
{
	EquippedItemDatas.RegisterEquipmentActor(EquipActor);
}

void USTInventoryComponent::UnregisterEquipmentActor(const FReplicatedItemData& RemovedItemData)
{
	EquippedItemDatas.UnregisterEquipmentActor(RemovedItemData);
}

void USTInventoryComponent::CloneAndMove(USTInventoryComponent* Clone)
{
	Clone->InventoryContainerSize = InventoryContainerSize;
	Clone->InventoryMaxColCount = InventoryMaxColCount;
	Clone->InventoryItemDatas = InventoryItemDatas;
	Clone->EquippedItemDatas = EquippedItemDatas;
	Clone->ReplicateInventory();

	InventoryContainerSize = 0;
	InventoryMaxColCount = 0;
	InventoryItemDatas = FInventoryContainer();
	EquippedItemDatas = FEquipmentContainer();
	ReplicateInventory();
}

void USTInventoryComponent::ReplicateInventory()
{
	MARK_PROPERTY_DIRTY_FROM_NAME(USTInventoryComponent, InventoryItemDatas, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(USTInventoryComponent, EquippedItemDatas, this);
}

void USTInventoryComponent::OnRep_InventorySizeChanged()
{
	InventoryItemDatas.SetContainerSize(InventoryContainerSize, InventoryMaxColCount);
}