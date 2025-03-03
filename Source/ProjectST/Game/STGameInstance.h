// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTST_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void ForceStreamingLoadAndCompile();
	
};
