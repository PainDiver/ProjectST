// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculation/STGameplayEffectEC_Regen_Stamina.h"
#include "GAS/STAttributeSet.h"
#include "Game/STNativeGameplayTag.h"

struct StaminaRegenCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(StaminaRegen);

	StaminaRegenCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USTAttributeSet, StaminaRegen, Source, false);
	}
};

static StaminaRegenCapture& GetStaminaRegenCapture()
{
	static StaminaRegenCapture RegenCapture;
	return RegenCapture;
}

USTGameplayEffectEC_Regen_Stamina::USTGameplayEffectEC_Regen_Stamina()
{
	RelevantAttributesToCapture.Add(GetStaminaRegenCapture().StaminaRegenDef);
}


void USTGameplayEffectEC_Regen_Stamina::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams,OutExecutionOutput);

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatState_Guard);
	BlockTags.AddTag(CombatState_Sprint);
	BlockTags.AddTag(Regen_Stop_Stamina);

	if (SourceTags->HasAny(BlockTags))
	{
		return;
	}

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float StaminaRegen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetStaminaRegenCapture().StaminaRegenDef, EvaluationParameters, StaminaRegen);

	if (StaminaRegen != 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			USTAttributeSet::GetCurrentStaminaAttribute(), EGameplayModOp::Additive, StaminaRegen));
	}

}
