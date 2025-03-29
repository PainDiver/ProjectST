// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "STGameplayEffectEC_Regen_Health.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTGameplayEffectEC_Regen_Health : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	

	USTGameplayEffectEC_Regen_Health();

	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;

	
	
};
