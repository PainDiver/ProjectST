// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Calculation/STGameplayEffectEC_Hit.h"
#include "GAS/STAttributeSet.h"
#include "Game/STNativeGameplayTag.h"

struct HitCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(Defense);

	HitCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(USTAttributeSet, CriticalChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USTAttributeSet, CriticalDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(USTAttributeSet, Defense, Target, false);
	}

};

static HitCapture& GetHitCapture()
{
	static HitCapture HitCapture;
	return HitCapture;
}

USTGameplayEffectEC_Hit::USTGameplayEffectEC_Hit()
{
	RelevantAttributesToCapture.Add(GetHitCapture().CriticalChanceDef);
	RelevantAttributesToCapture.Add(GetHitCapture().CriticalDamageDef);
	RelevantAttributesToCapture.Add(GetHitCapture().DefenseDef);
}

void USTGameplayEffectEC_Hit::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags =  Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float BaseDamage = Spec.GetSetByCallerMagnitude(EC_Damage);

	float CriticalChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHitCapture().CriticalChanceDef, EvaluationParameters, CriticalChance);

	float CriticalDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHitCapture().CriticalDamageDef, EvaluationParameters, CriticalDamage);

	float Defense = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetHitCapture().DefenseDef, EvaluationParameters, Defense);

	float CriticalCriteria = FMath::RandRange(0.f, 100.f);

	float FinalDamage = BaseDamage;
	if (CriticalCriteria < CriticalChance)
	{
		FinalDamage *= CriticalDamage / 100.f;
	}


	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(
		USTAttributeSet::GetCurrentHealthAttribute(), EGameplayModOp::Additive, -FinalDamage));
}
