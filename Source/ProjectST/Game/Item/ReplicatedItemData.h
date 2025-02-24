// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Misc/STEnum.h"
#include "Game/TableRows/Data_ItemInfoData.h"
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
	bool IsValid()const{ return ItemID > 0;}
	EItemUseType GetItemUseType() { return ItemInfo.ItemUseType; }

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
	int32 ItemID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid ItemUID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;

	// 가방안의 가방 같은 재귀는 안되는 구조, 따라서 Container타입금지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemContainerInfo> ContainingItems;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddItem, const FReplicatedItemData&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnModifyItem, const FReplicatedItemData&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveItem, const FReplicatedItemData&);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAddItem_Elem, const FReplicatedItemData&, ItemAdded);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnModifyItem_Elem, const FReplicatedItemData&, ItemModified);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnRemoveItem_Elem, const FReplicatedItemData&, ItemRemoved);


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

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);


	bool FindItem(FGuid UID, FReplicatedItemData& OutData);
	bool FindItem(int32 ID, TArray<FReplicatedItemData>& OutDatas);

	int32 AddItem(const FReplicatedItemData& NewData);

	bool ModifyItem(FGuid UID, const FReplicatedItemData& NewItemData);
	bool ModifyItem(const FReplicatedItemData& ItemToModify, const FReplicatedItemData& NewItemData);

	bool RemoveItem(const FReplicatedItemData& ItemToRemove, int32 Count);

	TArray<FReplicatedItemData>& GetArray() { return ItemData; }

	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);


	FOnAddItem& GetOnAddItem() { return OnAddItem; }
	FOnModifyItem& GetOnModifyItem() { return OnModifyItem; }
	FOnRemoveItem& GetOnRemoveItem() { return OnRemoveItem; }

protected:

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
// 아무래도 사람당 들고있는 아이템의 수가 많을 수 있으며,
// 다른사람이 알필요없으니, 필요한 상황일때만 리플리케이션 가능하도록
USTRUCT(Blueprintable, BlueprintType)
struct FInventoryContainer : public FReplicatedItemContainer
{
	GENERATED_BODY()

public:
	void Initialize(int32 Size);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);
	void RegisterInventorySearcher(UObject* Searcher);
	void UnregisterInventorySearcher(UObject* Searcher);


private:

	UPROPERTY(NotReplicated)
	int32 ContainerSize; 
	
	// 나중에 그리드로 변환가능
	// 따라서, EquipmentContainer는 그리드상관없이,
	// 인벤토리는 그리드로 표현가능해짐

	UPROPERTY(NotReplicated)
	TSet<UObject*> InventorySearchers;
};

template<>
struct TStructOpsTypeTraits<FInventoryContainer> : public TStructOpsTypeTraitsBase2<FInventoryContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};


// 2.
// Equipment Container, 장착한 아이템을 담는 컨테이너
USTRUCT(Blueprintable, BlueprintType)
struct FEquipmentContainer : public FReplicatedItemContainer
{
	GENERATED_BODY()

public:
	void EquipItem(const FReplicatedItemData& Item, FReplicatedItemContainer* SourceContainer);
	bool UnequipItem(EEquipSlotType SlotType, FReplicatedItemContainer* TargetContainer, FReplicatedItemData& OutTemp);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms);


private:

	UPROPERTY()
	TArray<EEquipSlotType> EquippedDesc;

	UPROPERTY(NotReplicated)
	uint8 ContainerSize = static_cast<uint8>(EEquipSlotType::MAX)-1;
};

template<>
struct TStructOpsTypeTraits<FEquipmentContainer> : public TStructOpsTypeTraitsBase2<FEquipmentContainer>
{
	enum
	{
		WithNetDeltaSerializer = true
	};
};

