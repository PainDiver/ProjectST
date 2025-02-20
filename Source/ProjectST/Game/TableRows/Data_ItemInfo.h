// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/STEnum.h"
#include "GameplayEffect.h"
#include "Data_ItemInfo.generated.h"

USTRUCT(BlueprintType)
struct FItemUseData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UGameplayAbility> UseAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UGameplayEffect> UseEffect;

};

USTRUCT(BlueprintType)
struct FItemInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemUseType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemUseData UseData;
};