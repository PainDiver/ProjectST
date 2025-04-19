// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/ReplicatedItemData.h"
#include "Data/DataTableManager.h"
#include "Game/STGameState.h"
#include "Game/Item/STEquipItemActor.h"

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
}

void FReplicatedItemData::SetData(FReplicatedItemData&& Item)
{
	ItemInfo = MoveTemp(Item.ItemInfo);
	ItemID = MoveTemp(Item.ItemID);
	ItemUID = MoveTemp(Item.ItemUID);
	ItemCount = MoveTemp(Item.ItemCount);
	ContainingItems = MoveTemp(Item.ContainingItems);
}

void FReplicatedItemData::Clear()
{
	ItemInfo = FItemInfoData();
	ItemID = 0;
	ItemUID = FGuid();
	ItemCount = 0;
	ContainingItems.Empty();
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
	return false;
}


int32 FReplicatedItemContainer::AddItem(const FReplicatedItemData& NewData)
{
	// Find Any Index

	int32 ChosenIndex = INDEX_NONE;
	FItemEquipInfoData EquipData;

	switch (ContainerType)
	{
		case EItemContainerType::EQUIPMENT:
		{
			UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
			if (TableManager == nullptr)
				return false;

			if (TableManager->GetTableData(TableManager->EquipItemDataTable, NewData.ItemInfo.ID, EquipData))
			{
				ChosenIndex = (uint8)EquipData.ItemSlotType;
			}
			break;
		}
		case EItemContainerType::INVENTORY:	
		{
			for (int32 i = 0; i < ItemData.Num(); i++)
			{
				if (!ItemData[i].IsValid())
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

	if (ChosenIndex < 0)
		return INDEX_NONE;

	ItemData[ChosenIndex].SetData(NewData);
	MarkItemDirty(ItemData[ChosenIndex]);	
	ASTGameState::RecordItemTrackingInfo(ItemData[ChosenIndex].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());
	OnAddItem.Broadcast(ChosenIndex, ItemData[ChosenIndex]);

	return ChosenIndex;
}

bool FReplicatedItemContainer::AddItemAt(int32 Index, const FReplicatedItemData& NewData)
{
	// Find Any Index

	FItemEquipInfoData EquipData;
	if (!ItemData.IsValidIndex(Index))
	{
		return false;
	}

	ItemData[Index].SetData(NewData);
	MarkItemDirty(ItemData[Index]);
	ASTGameState::RecordItemTrackingInfo(ItemData[Index].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());
	OnAddItem.Broadcast(Index, ItemData[Index]);
	return true;
}

void FReplicatedItemContainer::AddItemCount(int32 Index, int32 Count)
{
	ItemData[Index].ItemCount += Count;
	MarkItemDirty(ItemData[Index]);
	OnModifyItem.Broadcast(Index, ItemData[Index]);
}


bool FReplicatedItemContainer::ModifyItem(int32 Index, const FReplicatedItemData& NewItemData)
{
	if (ItemData.IsValidIndex(Index))
	{
		ItemData[Index].SetData(NewItemData);
		MarkItemDirty(ItemData[Index]);
		ASTGameState::RecordItemTrackingInfo(ItemData[Index].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());
		OnModifyItem.Broadcast(Index, ItemData[Index]);
		return true;
	}

	return false;
}

bool FReplicatedItemContainer::ModifyItem(FGuid UID, const FReplicatedItemData& NewItemData)
{
	for (int i=0 ; i< ItemData.Num();i++)
	{
		if (ItemData[i].ItemUID == UID)
		{
			ItemData[i].SetData(NewItemData);
			MarkItemDirty(ItemData[i]);
			ASTGameState::RecordItemTrackingInfo(ItemData[i].ItemUID, ItemTracing::HoldingContainerType::Container, this, FGuid());

			OnModifyItem.Broadcast(i, ItemData[i]);

			return true;
		}
	}

	return false;
}

bool FReplicatedItemContainer::ModifyItem(const FReplicatedItemData& ItemToModify, const FReplicatedItemData& NewItemData)
{
	return ModifyItem(ItemToModify.ItemUID, NewItemData);
}

bool FReplicatedItemContainer::RemoveItem(const FReplicatedItemData& ItemToRemove, int32 Count, FReplicatedItemData& OutRemoved)
{
	for (int i=0; i<ItemData.Num();i++)
	{
		if (ItemData[i] == ItemToRemove)
		{
			ItemData[i].ItemCount -= Count;
			if (ItemData[i].ItemCount <= 0)
			{
				ASTGameState::RemoveItemTrackingInfo(ItemData[i].ItemUID);
				OnRemoveItem.Broadcast(i,ItemData[i]);
				OutRemoved.SetData(ItemData[i]);
				ItemData[i].Clear();
				MarkItemDirty(ItemData[i]);
			}
			else
			{
				OutRemoved.SetData(ItemData[i]);
				OnModifyItem.Broadcast(i,ItemData[i]);
				MarkItemDirty(ItemData[i]);
			}
			return true;
		}
	}

	return false;
}

bool FReplicatedItemContainer::RemoveItem(int32 Index, int32 Count, FReplicatedItemData& OutRemoved)
{	
	return RemoveItem(ItemData[Index], Count, OutRemoved);
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
	ContainerSize = Size;
	for (int i = 0; i < Size; i++)
	{
		ItemData.AddDefaulted();
	}
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


