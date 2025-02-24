// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Game/DataAsset/STDataAsset_Input.h"
#include "Data_RootSkillSet.generated.h"


USTRUCT(BlueprintType)
struct PROJECTST_API FRootSkillSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<EInputType, TSubclassOf<UGameplayAbility>> Abilities;
};