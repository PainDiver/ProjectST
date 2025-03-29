// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculation/STGameplayEffectEC_Regen_Health.h"
#include "GAS/STAttributeSet.h"
#include "Game/STNativeGameplayTag.h"

struct HealthRegenCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(HealthRegen)

	HealthRegenCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USTAttributeSet,HealthRegen, Source,false)
	}
};

static HealthRegenCapture& GetHealthRegenCapture()
{
	static HealthRegenCapture RegenCapture;
	return RegenCapture;
}

USTGameplayEffectEC_Regen_Health::USTGameplayEffectEC_Regen_Health()
{
	RelevantAttributesToCapture.Add(GetHealthRegenCapture().HealthRegenDef);
}


void USTGameplayEffectEC_Regen_Health::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	const FGameplayEffectSpec& Spec =  ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(Regen_Stop_Health);

	if (SourceTags->HasAny(BlockTags))
	{
		return;
	}

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float HealthRegen = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHealthRegenCapture().HealthRegenDef, EvaluationParameters, HealthRegen);

	if (HealthRegen != 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
			USTAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Additive, HealthRegen));
	}
}
