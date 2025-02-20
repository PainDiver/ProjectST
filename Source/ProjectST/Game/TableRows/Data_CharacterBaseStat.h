// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data_CharacterBaseStat.generated.h"

USTRUCT(BlueprintType)
struct FCharacterBaseStat : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ID;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	int32 CurrentHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 MaxHealth;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SkillSetDataID;

};