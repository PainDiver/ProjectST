// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data_CharacterData.generated.h"

USTRUCT(BlueprintType)
struct PROJECTST_API FCharacterData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Desc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APawn> Class;
};