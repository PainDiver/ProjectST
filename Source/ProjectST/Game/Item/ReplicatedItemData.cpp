// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/ReplicatedItemData.h"
#include "Data/DataTableManager.h"
#include "Game/STGameState.h"


bool FReplicatedItemData::Initialize(UObject* Subject, int32 ID, int32 Count)
{
	if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager(Subject))
	{
		if (TableManager->GetItemInfoData(ID, ItemInfo) == false)
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
		if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager(Querier))
		{
			TableManager->GetItemInfoData(ItemID, ItemInfo);
		}
	}

	return ItemInfo;

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


int32 FReplicatedItemContainer::AddItem(const FReplicatedItemData& NewData)
{
	int32 Index = ItemData.Add(NewData);
	MarkItemDirty(ItemData[Index]);
	
	ASTGameState::RecordItemTrackingInfo(NewData.ItemUID, ItemTracing::ContainerType::Container, this, FGuid());

	OnAddItem.Broadcast(NewData);

	return Index;
}

bool FReplicatedItemContainer::ModifyItem(FGuid UID, const FReplicatedItemData& NewItemData)
{
	for (FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemUID == UID)
		{
			Item = NewItemData;
			MarkItemDirty(Item);
			ASTGameState::RecordItemTrackingInfo(Item.ItemUID, ItemTracing::ContainerType::Container, this, FGuid());

			return true;
		}
	}

	OnModifyItem.Broadcast(NewItemData);

	return false;
}

bool FReplicatedItemContainer::ModifyItem(const FReplicatedItemData& ItemToModify, const FReplicatedItemData& NewItemData)
{
	return ModifyItem(ItemToModify.ItemUID, NewItemData);
}

bool FReplicatedItemContainer::RemoveItem(const FReplicatedItemData& ItemToRemove, int32 Count)
{
	for (FReplicatedItemData& Item : ItemData)
	{
		if (Item == ItemToRemove)
		{
			Item.ItemCount -= Count;
			if (Item.ItemCount <= 0)
			{
				ASTGameState::RemoveItemTrackingInfo(Item.ItemUID);
				OnRemoveItem.Broadcast(Item);
				ItemData.Remove(Item);
				MarkArrayDirty();
			}
			else
			{
				OnModifyItem.Broadcast(Item);
				MarkItemDirty(Item);
			}

			return true;
		}
	}

	return false;
}


void FReplicatedItemContainer::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (int32 Indice : AddedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
			OnAddItem.Broadcast(ItemData[Indice]);
	}
}

void FReplicatedItemContainer::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	UE_LOG(LogTemp, Warning, TEXT("PreReplication"));

	for (int32 Indice : RemovedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
			OnRemoveItem.Broadcast(ItemData[Indice]);
	}
}

void FReplicatedItemContainer::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (int32 Indice : ChangedIndices)
	{
		if (ItemData.IsValidIndex(Indice))
			OnModifyItem.Broadcast(ItemData[Indice]);
	}
}




void FInventoryContainer::RegisterInventorySearcher(UObject* Searcher)
{
	InventorySearchers.Add(Searcher);
}

void FInventoryContainer::UnregisterInventorySearcher(UObject* Searcher)
{
	InventorySearchers.Remove(Searcher);
}


void FInventoryContainer::Initialize(int32 Size)
{
	ContainerSize = Size;
}


bool FReplicatedItemContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
}


bool FInventoryContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	if (OwningObject == DeltaParms.Object || InventorySearchers.Find(DeltaParms.Object))
	{
		return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
	}
	return false;
}

bool FEquipmentContainer::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
}

void FEquipmentContainer::EquipItem(const FReplicatedItemData& Item, FReplicatedItemContainer* SourceContainer)
{
	// 장착칸에 아이템 찾음, 스왑
	if (int32 Index = EquippedDesc.Find(Item.ItemInfo.ItemSlotType))
	{
		if (Index != INDEX_NONE && SourceContainer)
		{
			SourceContainer->ModifyItem(Item, ItemData[Index]);
		}
	}

	if (AddItem(Item) != INDEX_NONE)
	{
		EquippedDesc.Add(Item.ItemInfo.ItemSlotType);
	}
}

bool FEquipmentContainer::UnequipItem(EEquipSlotType SlotType, FReplicatedItemContainer* TargetContainer, FReplicatedItemData& OutTemp)
{
	if (int32 Index = EquippedDesc.Find(SlotType))
	{
		OutTemp = ItemData[Index];

		if(TargetContainer)
			TargetContainer->AddItem(ItemData[Index]);

		RemoveItem(ItemData[Index], 1);
		return true;
	}

	return false;
}


