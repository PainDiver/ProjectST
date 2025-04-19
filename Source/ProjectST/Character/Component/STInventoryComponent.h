// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/Item/ReplicatedItemData.h"
#include "Game/Item/STItemContainerInterface.h"
#include "STInventoryComponent.generated.h"


enum class EInventoryOperationType
{
	NONE,
	MOVE,
	SWAP,
	STACK
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTInventoryComponent : public UActorComponent, public ISTItemContainerInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	//UFUNCTION(BlueprintCallable)
	//void RegisterInventorySearcher(UObject* PS);
	//
	//UFUNCTION(BlueprintCallable)
	//void UnregisterInventorySearcher(UObject* PS);




	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RequestDragAndDrop_Server(
		USTInventoryComponent* SourceInventory,
		EItemContainerType SourceInventoryType,
		int32 SourceIndex,
		USTInventoryComponent* TargetInventory,
		EItemContainerType TargetInventoryType,
		int32 TargetIndex);

	bool CanDoDragAndDropOperation(
		EInventoryOperationType OperationType,
		EItemContainerType SourceInventoryType,
		const FItemInfoData& SourceItem,
		EItemContainerType TargetInventoryType,
		const FItemInfoData& TargetItem,
		EEquipSlotType TargetEquipSlotType
	);
	// Source아이템이 Equip인지
	// Target이 Equip인지,
	// Target에 


	UFUNCTION(BlueprintCallable)
	void DragAndDropItem(
		USTInventoryComponent* SourceInventory,
		EItemContainerType SourceInventoryType,
		int32 SourceIndex,
		USTInventoryComponent* TargetInventory,
		EItemContainerType TargetInventoryType,
		int32 TargetIndex);

	


	UFUNCTION(BlueprintCallable)
	void AcquireItem(const FReplicatedItemData& Item);

	//UFUNCTION(BlueprintCallable)
	//void DropItem(const FReplicatedItemData& Item);

	UFUNCTION(BlueprintPure)
	bool GetItemAt(EItemContainerType ContainerType,int32 Index,FReplicatedItemData& OutItem);

	// 내부사용
	virtual FReplicatedItemContainer* GetContainer(EItemContainerType ContainerType)override;



	// 블루프린트 노출용
	UFUNCTION(BlueprintPure)
	FInventoryContainer& GetInventoryComponent() { return InventoryItemDatas; }
	
	UFUNCTION(BlueprintPure)
	FEquipmentContainer& GetEquipmentComponent() { return EquippedItemDatas; }


	//UI, 외형같은 블루프린트 친화컨텐츠를 위한 바인딩용도로 주로쓰일예정
	UFUNCTION(BlueprintCallable)
	void BindOnAddItem(EItemContainerType ContainerType, FOnAddItem_Elem Delegate);
	UFUNCTION(BlueprintCallable)
	void BindOnRemoveItem(EItemContainerType ContainerType, FOnRemoveItem_Elem Delegate);
	UFUNCTION(BlueprintCallable)
	void BindOnModifyItem(EItemContainerType ContainerType, FOnModifyItem_Elem Delegate);

	UFUNCTION(BlueprintCallable)
	void RegisterEquipmentActor(ASTEquipItemActor* EquipActor);
	UFUNCTION(BlueprintCallable)
	void UnregisterEquipmentActor(const FReplicatedItemData& RemovedItemData);

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	int32 InventoryContainerSize;

	// 현재 지닌 아이템들의 데이터
	UPROPERTY(Replicated)
	FInventoryContainer InventoryItemDatas;

	// 장착 슬롯에 들어간 아이템들의 데이터
	UPROPERTY(Replicated)
	FEquipmentContainer EquippedItemDatas;

};
