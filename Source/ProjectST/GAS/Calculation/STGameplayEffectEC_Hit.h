// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "STGameplayEffectEC_Hit.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTGameplayEffectEC_Hit : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	USTGameplayEffectEC_Hit();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
	
};
