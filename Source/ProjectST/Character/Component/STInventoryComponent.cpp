// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STInventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/PlayerState.h"
#include "Misc/STEnum.h"
#include "Game/STGameState.h"

// Sets default values for this component's properties
USTInventoryComponent::USTInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
	InventoryContainerSize = 30;
	InventoryItemDatas.Initialize(InventoryContainerSize);
}


// Called when the game starts
void USTInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	// ...	
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

void USTInventoryComponent::RegisterInventorySearcher(UObject* PS)
{
	InventoryItemDatas.RegisterInventorySearcher(PS);
}

void USTInventoryComponent::UnregisterInventorySearcher(UObject* PS)
{
	InventoryItemDatas.UnregisterInventorySearcher(PS);
}

void USTInventoryComponent::EquipItemFromContainer(const FReplicatedItemData& Item, FReplicatedItemContainer& SourceContainer)
{
	EquippedItemDatas.EquipItem(Item, &SourceContainer);
}

void USTInventoryComponent::EquipItem(const FReplicatedItemData& Item)
{
	EquippedItemDatas.EquipItem(Item, nullptr);
}

void USTInventoryComponent::UnequipItemBySlot(EEquipSlotType SlotType, FReplicatedItemContainer& TargetContainer)
{
	FReplicatedItemData OutUnequipped;
	EquippedItemDatas.UnequipItem(SlotType, &TargetContainer, OutUnequipped);
}

void USTInventoryComponent::UnequipItemAndDrop(EEquipSlotType SlotType)
{
	FReplicatedItemData OutUnequipped;
	if (EquippedItemDatas.UnequipItem(SlotType, nullptr, OutUnequipped))
	{
		DropItem(OutUnequipped);
	}
}

void USTInventoryComponent::DropItem(const FReplicatedItemData& Item)
{
	if (ASTGameState* GameState = Cast<ASTGameState>(GetWorld()->GetGameState()))
	{
		//Set Drop Location or Drop 
		if (APawn* Pawn = Cast<APawn>(GetOwner()))
		{
			GameState->RealizeItemActor(Item, Pawn->GetActorLocation());
		}
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
		[MovedDelegate = MoveTemp(Delegate)](const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Data);
		}
	);
}

void USTInventoryComponent::BindOnRemoveItem(EItemContainerType ContainerType, FOnRemoveItem_Elem Delegate)
{
	GetContainer(ContainerType)->GetOnRemoveItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Data);
		}
	);

}

void USTInventoryComponent::BindOnModifyItem(EItemContainerType ContainerType, FOnModifyItem_Elem Delegate)
{
	GetContainer(ContainerType)->GetOnModifyItem().AddLambda(
		[MovedDelegate = MoveTemp(Delegate)](const FReplicatedItemData& Data)
		{
			MovedDelegate.ExecuteIfBound(Data);
		}
	);
}
