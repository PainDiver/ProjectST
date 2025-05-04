// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Game/STNativeGameplayTag.h"

void USTGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
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

void USTGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectContextHandle EffectContext = GetAbilitySystemComponentFromActorInfo()->MakeEffectContext();
		FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(CooldownGameplayEffectClass,0.f,EffectContext);
		if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
		{
			Spec->SetSetByCallerMagnitude(CooldownInfo.SetByCallerTag, CooldownInfo.Value);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void USTGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo,ActivationInfo,bReplicateEndAbility, bWasCancelled);

	if (AbilityTags.Num() > 0)
	{
		FGameplayEventData Data;
		Data.InstigatorTags = AbilityTags;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetAvatarActorFromActorInfo(), 
			bWasCancelled ? GA_Activation_Canceled : GA_Activation_Successful,
			Data);
	}
}

