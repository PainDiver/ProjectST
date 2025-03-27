// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Data/DataAsset/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "Data_RootSkillSet.generated.h"

USTRUCT(BlueprintType)
struct PROJECTST_API FInputBoundAbility : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EComboContextState State;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESTInputType InputType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability;
};

USTRUCT(BlueprintType)
struct PROJECTST_API FRootSkillSet : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Desc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TMap<ESTInputType, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FInputBoundAbility> AbilitiesToBind;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> AbilitiesToGive;
};