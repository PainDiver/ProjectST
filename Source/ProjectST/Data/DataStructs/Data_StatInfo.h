// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "Data_StatInfo.generated.h"

USTRUCT(BlueprintType)
struct PROJECTST_API FStatInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Desc;

	//SetByCaller
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FGameplayTag, float> StatValues;
};