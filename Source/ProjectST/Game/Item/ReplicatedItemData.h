// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Misc/STEnum.h"
#include "Data/DataStructs/Data_ItemInfoData.h"
#include "ReplicatedItemData.generated.h"

class USTItemObject;

USTRUCT(Blueprintable, BlueprintType)
struct FItemContainerInfo
{
	GENERATED_BODY()

public:
	FItemContainerInfo() = default;

	FItemContainerInfo(const FItemInfoData& Info, int32 ID,FGuid UID,int32 Count)
	{
		ItemInfo = Info;
		ItemID = ID;
		ItemUID = UID;
		ItemCount = Count;
	}
	bool operator==(const FItemContainerInfo& Other)
	{
		return ItemUID == Other.ItemUID;
	}

	UPROPERTY(NotReplicated, VisibleAnywhere, BlueprintReadOnly)
	FItemInfoData ItemInfo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid ItemUID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

USTRUCT(Blueprintable,BlueprintType)
struct FReplicatedItemData : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:

	bool Initialize(UObject* Subject,int32 ID,int32 Count);
	const FItemInfoData& GetItemInfo(UObject* Querier = nullptr);
	bool IsValid()const;
	EItemUseType GetItemUseType() { return ItemInfo.ItemUseType; }
	//리플리케이션하면 ItemInfo는 없으므로, ID로 찾기
	void FillItemInfo();

	void SetData(const FReplicatedItemData& Item);
	void SetData(FReplicatedItemData&& Item);

	void Clear();

	//가방안에 가방은 허용안할예정
	void AddAsContainer(FReplicatedItemContainer* Owner, const FReplicatedItemData& NewItem);
	void RemoveAsContainer(FReplicatedItemContainer* Owner, const FReplicatedItemData& Item);

	bool operator==(const FReplicatedItemData& Other)
	{
		return ItemUID == Other.ItemUID;
	}

	UPROPERTY(NotReplicated, VisibleAnywhere, BlueprintReadOnly)
	FItemInfoData ItemInfo;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int32 ItemID = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid ItemUID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount = 0;

	// Grid
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ParentIndex = INDEX_NONE;


	// 가방안의 가방 같은 재귀는 안되는 구조, 따라서 Container타입금지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemContainerInfo> ContainingItems;
};

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAddItem,int32,const FReplicatedItemData&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnModifyItem, int32,const FReplicatedItemData&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRemoveItem, int32,const FReplicatedItemData&);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAddItem_Elem, int32, Index,const FReplicatedItemData&, ItemAdded);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnModifyItem_Elem, int32, Index, const FReplicatedItemData&, ItemModified);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnRemoveItem_Elem, int32, Index, const FReplicatedItemData&, ItemRemoved);


USTRUCT(Blueprintable,BlueprintType)
struct FReplicatedItemContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:

	FReplicatedItemContainer() = default;
	FReplicatedItemContainer(const FReplicatedItemContainer&) = default;
	FReplicatedItemContainer& operator=(const FReplicatedItemContainer&) = default;
	FReplicatedItemContainer(FReplicatedItemContainer&&) = default;
	FReplicatedItemContainer& operator=(FReplicatedItemContainer&&) = default;

	void Initialize(int32 Size);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	bool FindItem(FGuid UID, FReplicatedItemData& OutData);
	bool FindItem(int32 ID, TArray<FReplicatedItemData>& OutDatas);
	bool FindItem(int32 Index, FReplicatedItemData& OutData);

	int32 AddItemAny(const FReplicatedItemData& NewData);
	bool AddItemAt(int32 Index, const FReplicatedItemData& NewData);
	void AddItemCount(int32 Index,int32 Count);	

	bool ModifyItemAt(int32 Index, const FReplicatedItemData& NewItemData);
	bool ModifyItemByUID(FGuid UID, const FReplicatedItemData& NewItemData);

	bool RemoveItemByItem(const FReplicatedItemData& ItemToRemove, int32 Count,FReplicatedItemData& OutRemoved);
	bool RemoveItemAt(int32 Index, int32 Count, FReplicatedItemData& OutRemoved);

	void SetContainerSize(int32 Size, int32 ColCount);
	FORCEINLINE int32 GetMaxRowCount()const { return (ContainerSize / MaxColCount) + 1; }

	bool GetGridSpaceFromIndex(int32 Index, const FIntPoint& Size, TArray<int32>& OutNeighborIndices);
	bool CheckSpaceForInventory(int32 Index,const FIntPoint& Size, const FReplicatedItemData& IgnoredItem, TArray<int32>& OutNeighborIndices);

	TArray<FReplicatedItemData>& GetArray() { return ItemData; }

	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	FOnAddItem& GetOnAddItem() { return OnAddItem; }
	FOnModifyItem& GetOnModifyItem() { return OnModifyItem; }
	FOnRemoveItem& GetOnRemoveItem() { return OnRemoveItem; }

	EItemContainerType GetContainerType() { return ContainerType; }

private:
	bool AddItem(int32 Index, const FReplicatedItemData& NewData);
	bool ModifyItem(int32 Index, const FReplicatedItemData& NewItemData);
	bool RemoveItem(int32 Index, int32 Count, FReplicatedItemData& OutRemoved);


protected:

	UPROPERTY()
	int32 MaxColCount;

	UPROPERTY()
	int32 ContainerSize;

	UPROPERTY(NotReplicated)
	EItemContainerType ContainerType;

	FOnAddItem OnAddItem;

	FOnModifyItem OnModifyItem;

	FOnRemoveItem OnRemoveItem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FReplicatedItemData> ItemData;
};

template<>
struct TStructOpsTypeTraits<FReplicatedItemContainer> : public TStructOpsTypeTraitsBase2<FReplicatedItemContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

// 1.
// Inventory Container, 일반 아이템을담는 인벤토리 컨테아너
USTRUCT(Blueprintable, BlueprintType)
struct FInventoryContainer : public FReplicatedItemContainer
{
	GENERATED_BODY()

public:
	FInventoryContainer();
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
	
private:

};

template<>
struct TStructOpsTypeTraits<FInventoryContainer> : public TStructOpsTypeTraitsBase2<FInventoryContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


class ASTEquipItemActor;
// 2.
// Equipment Container, 장착한 아이템을 담는 컨테이너
USTRUCT(Blueprintable, BlueprintType)
struct FEquipmentContainer : public FReplicatedItemContainer
{
	GENERATED_BODY()

public:
	FEquipmentContainer();
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);

	void RegisterEquipmentActor(ASTEquipItemActor* Actor);
	void UnregisterEquipmentActor(const FReplicatedItemData& RemoveItem);

private:

	UPROPERTY()
	TArray<ASTEquipItemActor*> EquipmentActor;
};

template<>
struct TStructOpsTypeTraits<FEquipmentContainer> : public TStructOpsTypeTraitsBase2<FEquipmentContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

