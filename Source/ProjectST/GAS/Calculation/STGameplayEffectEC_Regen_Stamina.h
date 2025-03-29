// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "STGameplayEffectEC_Regen_Stamina.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTGameplayEffectEC_Regen_Stamina : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	USTGameplayEffectEC_Regen_Stamina();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;	
};
