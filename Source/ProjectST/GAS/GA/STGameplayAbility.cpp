// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameplayAbility.h"
#include "AbilitySystemComponent.h"


void USTGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}


bool USTGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (RequiredStats.Num() > 0)
	{
		for (const TPair<FGameplayAttribute, float>& RequiredStat : RequiredStats)
		{
			float Result = 0.f;
			if (ActorInfo->AbilitySystemComponent->HasAttributeSetForAttribute(RequiredStat.Key))
			{
				Result = ActorInfo->AbilitySystemComponent->GetNumericAttributeBase(RequiredStat.Key);
				if (Result < RequiredStat.Value)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	return true;
}

void USTGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CostGameplayEffectClass)
	{
		FGameplayEffectSpecHandle EffectHandle = MakeOutgoingGameplayEffectSpec(CostGameplayEffectClass);
		if (FGameplayEffectSpec* Spec = EffectHandle.Data.Get())
		{
			for (const FSTAbilityCost& Cost : CostStats)
			{
				Spec->SetSetByCallerMagnitude(Cost.SetByCallerTag,-Cost.Value);
			}
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, EffectHandle);
		}
	}
}

