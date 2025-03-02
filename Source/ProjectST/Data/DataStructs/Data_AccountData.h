// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data_AccountData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct PROJECTST_API FAccountData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 UserUID = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString LoginToken;

	// -> Character Table
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ChosenCharacterID;

};
