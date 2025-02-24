// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data_CharacterBaseStat.generated.h"

USTRUCT(BlueprintType)
struct PROJECTST_API FCharacterBaseStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SkillSetDataID;

};