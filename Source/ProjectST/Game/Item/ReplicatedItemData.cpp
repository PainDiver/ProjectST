// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/ReplicatedItemData.h"

bool FReplicatedItemContainer::FindItem(const FGuid& ItemUID, FReplicatedItemData& OutData)
{
	for (const FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemUID == ItemUID)
		{
			OutData = Item;
			return true;
		}
	}

	return false;
}

void FReplicatedItemContainer::AddItem(const FReplicatedItemData& NewData)
{
	ItemData.Add(NewData);
	MarkItemDirty(ItemData.Last());
}

bool FReplicatedItemContainer::ModifyItem(const FGuid& ItemUID, const FReplicatedItemData& NewItemData)
{
	for (FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemUID == ItemUID)
		{
			Item = NewItemData;
			MarkItemDirty(Item);
			return true;
		}
	}

	return false;
}

bool FReplicatedItemContainer::RemoveItem(const FGuid& ItemUID, int32 Count)
{
	for (FReplicatedItemData& Item : ItemData)
	{
		if (Item.ItemUID == ItemUID)
		{
			Item.Count -= Count;
			if (Item.Count <= 0)
			{
				ItemData.Remove(Item);
				MarkArrayDirty();
			}
			else
			{
				MarkItemDirty(Item);
			}
			return true;
		}
	}

	return false;
}