// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/Item/STItemActor.h"
#include "STEquipItemActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTEquipItemActor : public ASTItemActor
{
	GENERATED_BODY()
	
	
public:

	ASTEquipItemActor(const FObjectInitializer& OI);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void Initialize(AActor* Avatar, const FReplicatedItemData& Data, const FItemEquipInfoData& EquipData);
	void Initialize_Implementation(AActor* Avatar, const FReplicatedItemData& Data,const FItemEquipInfoData& EquipData);

	// 상세한 동작 및, 따로 로직을 구현할때는 BP 활용

protected:

	FItemEquipInfoData EquipInfo;

};
