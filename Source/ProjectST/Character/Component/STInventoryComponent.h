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

	UFUNCTION(BlueprintCallable)
	void DragAndDropItem(
		USTInventoryComponent* SourceInventory,
		EItemContainerType SourceInventoryType,
		int32 SourceIndex,
		USTInventoryComponent* TargetInventory,
		EItemContainerType TargetInventoryType,
		int32 TargetIndex);

	UFUNCTION(BlueprintPure)
	bool CanStack(const FReplicatedItemData& SourceItemData, const FReplicatedItemData& TargetItemData);

	EInventoryOperationType SelectOperation(
		const FReplicatedItemData& SourceItemData,
		FReplicatedItemContainer* SourceContainer,
		int32 SourceIndex,
		const FReplicatedItemData& TargetItemData,
		FReplicatedItemContainer* TargetContainer,
		int32 TargetIndex
	);

	UFUNCTION(Server,Reliable,BlueprintCallable)
	void RequestSplit(
		USTInventoryComponent* SourceInventory,
		EItemContainerType SourceInventoryType,
		int32 SourceIndex,
		USTInventoryComponent* TargetInventory,
		EItemContainerType TargetInventoryType,
		int32 TargetIndex,
		int32 CountToSplit);



	UFUNCTION(BlueprintCallable)
	void AcquireItem(const FReplicatedItemData& Item);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RequestDropItem_Server(EItemContainerType ContainerType, const FGuid& ItemUID);

	UFUNCTION(BlueprintCallable)
	bool DropItem(EItemContainerType ContainerType,const FReplicatedItemData& Item);

	//UFUNCTION(BlueprintCallable)
	//void DropItem(const FReplicatedItemData& Item);

	UFUNCTION(BlueprintPure)
	bool GetItemAt(EItemContainerType ContainerType,int32 Index,FReplicatedItemData& OutItem);

	// 내부사용
	virtual FReplicatedItemContainer* GetContainer(EItemContainerType ContainerType)override;

	UFUNCTION(BlueprintCallable)
	bool CheckSpaceForInventory(int32 Index, const FIntPoint& Size, const FReplicatedItemData& IgnoredItem, TArray<int32>& OutNeighbors);


	// 블루프린트 노출용
	UFUNCTION(BlueprintPure)
	FInventoryContainer& GetInventoryComponent() { return InventoryItemDatas; }
	
	UFUNCTION(BlueprintPure)
	FEquipmentContainer& GetEquipmentComponent() { return EquippedItemDatas; }

	UFUNCTION(BlueprintPure)
	AActor* GetOwnerActor()const;

	//UI, 외형같은 블루프린트 친화컨텐츠를 위한 바인딩용도로 주로쓰일예정
	UFUNCTION(BlueprintCallable)
	FBlueprintExposedDelegateHandle BindOnAddItem(EItemContainerType ContainerType, FOnAddItem_Elem Delegate);
	UFUNCTION(BlueprintCallable)
	FBlueprintExposedDelegateHandle BindOnRemoveItem(EItemContainerType ContainerType, FOnRemoveItem_Elem Delegate);
	UFUNCTION(BlueprintCallable)
	FBlueprintExposedDelegateHandle BindOnModifyItem(EItemContainerType ContainerType, FOnModifyItem_Elem Delegate);

	UFUNCTION(BlueprintCallable)
	void ClearOnAddItem(EItemContainerType ContainerType,const FBlueprintExposedDelegateHandle& DelegateHandle);
	UFUNCTION(BlueprintCallable)
	void ClearOnRemoveItem(EItemContainerType ContainerType, const FBlueprintExposedDelegateHandle& DelegateHandle);
	UFUNCTION(BlueprintCallable)
	void ClearOnModifyItem(EItemContainerType ContainerType, const FBlueprintExposedDelegateHandle& DelegateHandle);

	UFUNCTION(BlueprintCallable)
	void RegisterEquipmentActor(ASTEquipItemActor* EquipActor);
	UFUNCTION(BlueprintCallable)
	void UnregisterEquipmentActor(const FReplicatedItemData& RemovedItemData);

	void CloneAndMove(USTInventoryComponent* Clone);

	void ReplicateInventory();

	UFUNCTION()
	void OnRep_InventorySizeChanged();

private:
	UPROPERTY(ReplicatedUsing = OnRep_InventorySizeChanged,EditAnywhere, BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	int32 InventoryContainerSize;

	UPROPERTY(ReplicatedUsing = OnRep_InventorySizeChanged, EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 InventoryMaxColCount = 7;

	// 현재 지닌 아이템들의 데이터
	UPROPERTY(Replicated)
	FInventoryContainer InventoryItemDatas;

	// 장착 슬롯에 들어간 아이템들의 데이터
	UPROPERTY(Replicated)
	FEquipmentContainer EquippedItemDatas;

};
