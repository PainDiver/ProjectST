// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "STGameplayEffectEC_SprintCost.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTGameplayEffectEC_SprintCost : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:

	//USTGameplayEffectEC_SprintCost();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;

	
};
