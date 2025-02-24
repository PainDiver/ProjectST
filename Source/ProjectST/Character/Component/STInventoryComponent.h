// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Game/Item/ReplicatedItemData.h"
#include "Game/Item/STItemContainerInterface.h"
#include "STInventoryComponent.generated.h"


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

	UFUNCTION(BlueprintCallable)
	void RegisterInventorySearcher(UObject* PS);
	
	UFUNCTION(BlueprintCallable)
	void UnregisterInventorySearcher(UObject* PS);


	UFUNCTION(BlueprintCallable)
	void EquipItemFromContainer(const FReplicatedItemData& Item, UPARAM(ref) FReplicatedItemContainer& SourceContainer);
	UFUNCTION(BlueprintCallable)
	void EquipItem(const FReplicatedItemData& Item);

	UFUNCTION(BlueprintCallable)
	void UnequipItemBySlot(EEquipSlotType SlotType,UPARAM(ref) FReplicatedItemContainer& TargetContainer);
	UFUNCTION(BlueprintCallable)
	void UnequipItemAndDrop(EEquipSlotType SlotType);

	UFUNCTION(BlueprintCallable)
	void DropItem(const FReplicatedItemData& Item);

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
