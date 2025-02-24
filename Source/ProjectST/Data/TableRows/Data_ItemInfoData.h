// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/STEnum.h"
#include "GameplayEffect.h"
#include "Data_ItemInfoData.generated.h"

USTRUCT(BlueprintType)
struct FItemUseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> UseAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> UseEffect;
};


USTRUCT(BlueprintType)
struct PROJECTST_API FItemInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemUseType ItemUseType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEquipSlotType ItemSlotType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemUseData UseData;
};


