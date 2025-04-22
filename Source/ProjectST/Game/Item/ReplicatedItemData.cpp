// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/ReplicatedItemData.h"
#include "Data/DataTableManager.h"
#include "Game/STGameState.h"
#include "Game/Item/STEquipItemActor.h"
#include "Misc/STGameBlueprintFunctionLibrary.h"


FReplicatedItemData FReplicatedItemData::EmptyData = FReplicatedItemData();

bool FReplicatedItemData::Initialize(UObject* Subject, int32 ID, int32 Count)
{
	if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager())
	{
		if (TableManager->GetTableData(TableManager->ItemInfoDataTable,ID, ItemInfo) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Item Not Initialized "));
			return false;
		}

		ItemID = ID;
		ItemUID = FGuid::NewGuid();
		ItemCount = Count;
		return true;
	}
	
	return false;

}

void FReplicatedItemData::AddAsContainer(FReplicatedItemContainer* Owner, const FReplicatedItemData& NewItem)
{
	if (NewItem.ItemInfo.ItemUseType == EItemUseType::CONTAINER)
		return;


	if (ContainingItems.Emplace(NewItem.ItemInfo, NewItem.ItemID, NewItem.ItemUID, NewItem.ItemCount) != INDEX_NONE)
	{
		if (Owner)
		{
			ASTGameState::RecordItemTrackingInfo(NewItem.ItemUID,ItemTracing::Item,Owner,ItemUID);
			Owner->MarkItemDirty(*this);
		}
	}
}

void FReplicatedItemData::RemoveAsContainer(FReplicatedItemContainer* Owner, const FReplicatedItemData& Item)
{
	FItemContainerInfo ItemToRemove = { Item.ItemInfo,Item.ItemID,Item.ItemUID,Item.ItemCount };
	if (ContainingItems.Remove(ItemToRemove) != INDEX_NONE)
	{
		if (Owner)
		{
			ASTGameState::RemoveItemTrackingInfo(Item.ItemUID);
			Owner->MarkItemDirty(*this);
		}
	}
}


const FItemInfoData& FReplicatedItemData::GetItemInfo(UObject* Querier)
{	
	if (ItemID != 0 && ItemInfo.ID == 0)
	{
		if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager())
		{
			TableManager->GetTableData(TableManager->ItemInfoDataTable,ItemID, ItemInfo);
		}
	}

	return ItemInfo;

}

bool FReplicatedItemData::IsValid() const
{
	return (ItemID > 0 && ItemUID.IsValid()) || ParentIndex != -1;
}

void FReplicatedItemData::FillItemInfo()
{	
	if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager())
	{
		FItemInfoData OutData;
		if (TableManager->GetTableData(TableManager->ItemInfoDataTable, ItemID, OutData))
		{
			ItemInfo = OutData;
		}
	}
}

void FReplicatedItemData::SetData(const FReplicatedItemData& Item)
{
	ItemInfo = Item.ItemInfo;
	ItemID = Item.ItemID;
	ItemUID = Item.ItemUID;
	ItemCount = Item.ItemCount;
	ContainingItems = Item.ContainingItems;
	ParentIndex = Item.ParentIndex;
}

void FReplicatedItemData::SetData(FReplicatedItemData&& Item)
{
	ItemInfo = MoveTemp(Item.ItemInfo);
	ItemID = MoveTemp(Item.ItemID);
	ItemUID = MoveTemp(Item.ItemUID);
	ItemCount = MoveTemp(Item.ItemCount);
	ContainingItems = MoveTemp(Item.ContainingItems);
	ParentIndex = MoveTemp(Item.ParentIndex);

}

void FReplicatedItemData::Clear()
{
	ItemInfo = FItemInfoData();
	ItemID = 0;
	ItemUID = FGuid();
	ItemCount = 0;
	ContainingItems.Empty();
	ParentIndex = INDEX_NONE;
}

bool FReplicatedItemContainer::FindItem(FGuid UID, FReplicatedItemData& OutData)
{
	for (const FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemUID == UID)
		{
			OutData = Item;
			return true;
		}
	}
	OutData = FReplicatedItemData::EmptyData;

	return false;
}

bool FReplicatedItemContainer::FindItem(int32 ID, TArray<FReplicatedItemData>& OutDatas)
{
	for (const FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemID == ID)
		{
			OutDatas.Add(Item);
		}
	}

	return OutDatas.Num() > 0;
}

bool FReplicatedItemContainer::FindItem(int32 Index, FReplicatedItemData& OutData)
{
	if (ItemData.IsValidIndex(Index) && ItemData[Index].IsValid())
	{
		OutData = ItemData[Index];
		return true;
	}
	OutData = FReplicatedItemData::EmptyData;
	return false;
}


int32 FReplicatedItemContainer::AddItemAny(const FReplicatedItemData& NewData)
{
	// Find Any Index

	int32 ChosenIndex = INDEX_NONE;
	switch (ContainerType)
	{
		case EItemContainerType::EQUIPMENT:
		{
			UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
			if (TableManager == nullptr)
				return false;

			FItemEquipInfoData EquipData;
			if (TableManager->GetTableData(TableManager->EquipItemDataTable, NewData.ItemInfo.EquipInfoID, EquipData))
			{
				ChosenIndex = (uint8)EquipData.ItemSlotType;
			}

			if (!AddItem(ChosenIndex, NewData))
			{
				ChosenIndex = INDEX_NONE;
			}
			break;
		}
		case EItemContainerType::INVENTORY:	
		{
			for (int32 i = 0; i < ItemData.Num(); i++)
			{
				if (AddItem(i,NewData))
				{
					ChosenIndex = i;
					break;
				}
			}
			break;
		}
		default:			
			break;
	}

	return ChosenIndex;
}

bool FReplicatedItemContainer::AddItemAt(int32 Index, const FReplicatedItemData& NewData)
{
	AddItem(Index, NewData);
	return true;
}

void FReplicatedItemContainer::AddItemCount(int32 Index, int32 Count)
{
	int32 ParentIndex = ItemData[Index].ParentIndex;

	ItemData[ParentIndex].ItemCount += Count;
	MarkItemDirty(ItemData[ParentIndex]);
	OnModifyItem.Broadcast(ParentIndex, ItemData[ParentIndex]);
}

bool FReplicatedItemContainer::ModifyItemAt(int32 Index, const FReplicatedItemData& NewItemData)
{	
	ModifyItem(ItemData[Index].ParentIndex, NewItemData);
	return true;
}

bool FReplicatedItemContainer::ModifyItemByUID(FGuid UID, const FReplicatedItemData& NewItemData)
{
	for (int i=0 ; i< ItemData.Num();i++)
	{
		if (ItemData[i].ItemUID == UID)
		{
			ModifyItem(i, NewItemData);
			return true;
		}
	}

	return false;
}

bool FReplicatedItemContainer::RemoveItemByItem(const FReplicatedItemData& ItemToRemove, int32 Count, FReplicatedItemData& OutRemoved)
{
	for (int i=0; i<ItemData.Num();i++)
	{
		if (ItemData[i] == ItemToRemove)
		{
			return RemoveItem(i, Count, OutRemoved);
		}
	}

	return false;
}

bool FReplicatedItemContainer::RemoveItemAt(int32 Index, int32 Count, FReplicatedItemData& OutRemoved)
{		
	return RemoveItem(ItemData[Index].ParentIndex, Count, OutRemoved);
}

void FReplicatedItemContainer::SetContainerSize(int32 Size, int32 ColCount)
{
	ContainerSize = Size;
	MaxColCount = ColCount;
}


bool FReplicatedItemContainer::AddItem(int32 Index, const FReplicatedItemData& NewData)
{
	if (!ItemData.IsValidIndex(Index))
	{
		return false;
	}

	TArray<int32> Neighbors;
	if (ContainerType == EItemContainerType::INVENTORY)
	{
		if (!CheckSpaceForInventory(Index,NewData.ItemInfo.ItemSize,NewData,Neighbors))
		{
			return false;
		}
	}

	ItemData[Index].SetData(NewData);
	MarkItemDirty(ItemData[Index]);
	if (Neighbors.Num() > 0)
	{
		for (int32 GridIndex : Neighbors)
		{
			ItemData[GridIndex].ParentIndex = Index;
			MarkItemDirty(ItemData[GridIndex]);
		}
	}
	else
	{
		ItemData[Index].ParentIndex = Index;
		MarkItemDirty(ItemData[Index]);
	}

	ASTGameState::RecordItemTrackingInfo(ItemData[Index].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());
	OnAddItem.Broadcast(Index, ItemData[Index]);

	return true;
}

bool FReplicatedItemContainer::ModifyItem(int32 Index, const FReplicatedItemData& NewItemData)
{
	if (!ItemData.IsValidIndex(Index))
	{
		return false;
	}

	TArray<int32> Neighbors;
	if (ContainerType == EItemContainerType::INVENTORY)
	{
		GetGridSpaceFromIndex(Index,ItemData[Index].ItemInfo.ItemSize, Neighbors);
	}

	ItemData[Index].SetData(NewItemData);
	MarkItemDirty(ItemData[Index]);
	if (Neighbors.Num() > 0)
	{
		for (int32 GridIndex : Neighbors)
		{
			ItemData[GridIndex].ParentIndex = Index;
			MarkItemDirty(ItemData[GridIndex]);
		}
	}
	else
	{
		ItemData[Index].ParentIndex = Index;
		MarkItemDirty(ItemData[Index]);
	}
	ASTGameState::RecordItemTrackingInfo(ItemData[Index].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());
	OnModifyItem.Broadcast(Index, ItemData[Index]);
	return true;
}

bool FReplicatedItemContainer::RemoveItem(int32 Index, int32 Count, FReplicatedItemData& OutRemoved)
{
	if (!ItemData.IsValidIndex(Index))
	{
		return false;
	}

	TArray<int32> Neighbors;
	if (ContainerType == EItemContainerType::INVENTORY)
	{
		GetGridSpaceFromIndex(Index, ItemData[Index].ItemInfo.ItemSize, Neighbors);
	}

	int32 ItemCount = ItemData[Index].ItemCount;
	ItemCount -= Count;
	if (ItemCount <= 0)
	{
		ASTGameState::RemoveItemTrackingInfo(ItemData[Index].ItemUID);
		OutRemoved.SetData(ItemData[Index]);
		ItemData[Index].Clear();		
		MarkItemDirty(ItemData[Index]);
		for (int32 GridIndex : Neighbors)
		{
			ItemData[GridIndex].Clear();
			MarkItemDirty(ItemData[GridIndex]);
		}
		OnRemoveItem.Broadcast(Index, OutRemoved);
	}
	else
	{
		ItemData[Index].ItemCount = ItemCount;
		OutRemoved.SetData(ItemData[Index]);
		OnModifyItem.Broadcast(Index, ItemData[Index]);
		MarkItemDirty(ItemData[Index]);
	}
	return true;

}




bool FReplicatedItemContainer::GetGridSpaceFromIndex(int32 Index, const FIntPoint& Size, TArray<int32>& OutNeighborIndices)
{
	TArray<FIntPoint> Points;
	TArray<int32> Indices;
	if (USTGameBlueprintFunctionLibrary::GetGridPointsFromOffset(Points, Indices, Index, MaxColCount, GetMaxRowCount(), Size))
	{		
		OutNeighborIndices = Indices;
		return true;
	}

	return false;
}

bool FReplicatedItemContainer::CheckSpaceForInventory(int32 Index, const FIntPoint& Size, const FReplicatedItemData& IgnoredItem, TArray<int32>& OutNeighborIndices)
{	
	if (MaxColCount == 0)
		return false;

	TArray<FIntPoint> Points;
	TArray<int32> Indices;		

	if (ContainerType == EItemContainerType::INVENTORY)
	{
		if (USTGameBlueprintFunctionLibrary::GetGridPointsFromOffset(Points, Indices, Index, MaxColCount, GetMaxRowCount(), Size))
		{
			bool bMetCondition = true;
			int32 SharedParentIndex = INDEX_NONE;
			bool bSharedParentChanged = false;
			for (int32 GridIndex : Indices)
			{
				if (!ItemData.IsValidIndex(GridIndex))
				{
					bMetCondition = false;
					break;
				}
				else if (ItemData[GridIndex].IsValid())
				{		
					if (IgnoredItem.IsValid())
					{
						if (IgnoredItem != ItemData[ItemData[GridIndex].ParentIndex])
						{
							bMetCondition = false;
							break;
						}
					}
					else
					{
						bMetCondition = false;
						break;
					}
				}

			}
			OutNeighborIndices = Indices;
			return bMetCondition;
		}
	}
	else
	{
		if (ItemData.IsValidIndex(Index))
		{
			return !ItemData[Index].IsValid();
		}
	}

	return false;
}

void FReplicatedItemContainer::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Indice : AddedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
		{
			ItemData[Indice].FillItemInfo();
			OnAddItem.Broadcast(Indice,ItemData[Indice]);
		}
	}
}

void FReplicatedItemContainer::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (int32 Indice : RemovedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
		{
			ItemData[Indice].FillItemInfo();
			OnRemoveItem.Broadcast(Indice,ItemData[Indice]);
		}
	}
}

void FReplicatedItemContainer::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Indice : ChangedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
		{
			ItemData[Indice].FillItemInfo();
			OnModifyItem.Broadcast(Indice,ItemData[Indice]);
		}
	}
}

//void FInventoryContainer::RegisterInventorySearcher(UObject* Searcher)
//{
//	InventorySearchers.Add(Searcher);
//	MarkArrayDirty();
//}
//
//void FInventoryContainer::UnregisterInventorySearcher(UObject* Searcher)
//{
//	InventorySearchers.Remove(Searcher);
//}


FInventoryContainer::FInventoryContainer()
	:FReplicatedItemContainer()
{
	ContainerType = EItemContainerType::INVENTORY;
}

void FReplicatedItemContainer::Initialize(int32 Size)
{	
	for (int i = 0; i < Size; i++)
	{
		if(!ItemData.IsValidIndex(i))
			ItemData.AddDefaulted();
	}
	ItemData.Shrink();
	MarkArrayDirty();
}

bool FReplicatedItemContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
}


bool FInventoryContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
}

bool FEquipmentContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
}

void FEquipmentContainer::RegisterEquipmentActor(ASTEquipItemActor* Actor)
{
	EquipmentActor.AddUnique(Actor);
}

void FEquipmentContainer::UnregisterEquipmentActor(const FReplicatedItemData& RemoveItem)
{
	for (ASTEquipItemActor* Actor : EquipmentActor)
	{
		if (Actor->GetItemData().ItemUID == RemoveItem.ItemUID)
		{
			Actor->OnUnequipItem(Cast<USTInventoryComponent>(OwningObject));
			EquipmentActor.Remove(Actor);
			return;
		}
	}
}


FEquipmentContainer::FEquipmentContainer()
{
	ContainerType = EItemContainerType::EQUIPMENT;
}


