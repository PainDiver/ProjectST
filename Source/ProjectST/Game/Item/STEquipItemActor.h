// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Item/STItemActor.h"
#include "Character/Component/STInventoryComponent.h"
#include "STEquipItemActor.generated.h"

/**
 * 
 */
class USTInventoryComponent;
UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTEquipItemActor : public ASTItemActor
{
	GENERATED_BODY()
	
	
public:

	ASTEquipItemActor(const FObjectInitializer& OI);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void InitEquipItem(AActor* Avatar, const FReplicatedItemData& Data, const FItemEquipInfoData& EquipData);
	void InitEquipItem_Implementation(AActor* Avatar, const FReplicatedItemData& Data, const FItemEquipInfoData& EquipData);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnUnequipItem(USTInventoryComponent* Inventory);
	void OnUnequipItem_Implementation(USTInventoryComponent* Inventory) {};

	UFUNCTION(BlueprintPure)
	EEquipSlotType GetSlotType() { return SlotType; }

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EEquipSlotType SlotType;

	FItemEquipInfoData EquipInfo;

};
