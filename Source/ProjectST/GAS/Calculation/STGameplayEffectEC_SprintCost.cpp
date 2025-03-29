// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculation/STGameplayEffectEC_SprintCost.h"
#include "Game/STNativeGameplayTag.h"
#include "GAS/STAttributeSet.h"
#include "Character/STStateInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

//struct SprintCostCapture
//{	
//	DECLARE_ATTRIBUTE_CAPTUREDEF();
//	
//	SprintCostCapture()
//	{
//		DEFINE_ATTRIBUTE_CAPTUREDEF();
//	}
//};
//
//static SprintCostCapture& GetSprintCostCapture()
//{
//	static SprintCostCapture Capture;
//	return Capture;
//}
//
//USTGameplayEffectEC_SprintCost::USTGameplayEffectEC_SprintCost()
//{
//	RelevantAttributesToCapture.Add();
//}

void USTGameplayEffectEC_SprintCost::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FGameplayTagContainer RequiredTags;
	RequiredTags.AddTag(CombatState_Sprint);
	
	if (!SourceTags->HasAll(RequiredTags))
	{
		return;
	}

	//FAggregatorEvaluateParameters EvaluationParameters;
	//EvaluationParameters.SourceTags;
	//EvaluationParameters.TargetTags;

	//float Stat;
	//ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetSprintCostCapture()., EvaluationParameters,Stat);

	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		USTAttributeSet::GetCurrentStaminaAttribute(),
		EGameplayModOp::Additive, 
		-20.f));
}

