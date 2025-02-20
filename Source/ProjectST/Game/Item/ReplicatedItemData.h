// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "ReplicatedItemData.generated.h"

USTRUCT(Blueprintable,BlueprintType)
struct FReplicatedItemData : public FFastArraySerializerItem
{
	GENERATED_BODY()

	//아이템의 동적데이터만
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FGuid ItemUID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	AActor* Owner;

	bool operator==(const FReplicatedItemData& Other)
	{
		return ItemUID == Other.ItemUID;
	}
};

USTRUCT()
struct FReplicatedItemContainer : public FFastArraySerializer
{
	GENERATED_BODY()

public:

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FReplicatedItemData>(ItemData, DeltaParms, *this);
	}

	bool FindItem(const FGuid& ItemUID, FReplicatedItemData& OutData);

	void AddItem(const FReplicatedItemData& NewData);

	bool ModifyItem(const FGuid& ItemUID, const FReplicatedItemData& NewItemData);

	bool RemoveItem(const FGuid& ItemUID, int32 Count);

private:
	UPROPERTY()
	TArray<FReplicatedItemData> ItemData;
};

template<>
struct TStructOpsTypeTraits<FReplicatedItemContainer> : public TStructOpsTypeTraitsBase2<FReplicatedItemContainer>
{
	enum
	{
		WithNetDeltaSerializer = false
	};
};
