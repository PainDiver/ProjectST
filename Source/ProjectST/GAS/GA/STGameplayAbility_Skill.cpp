// Fill out your copyright notice in the Description page of Project Settings.


#include "STGameplayAbility_Skill.h"

TSubclassOf<UGameplayAbility> USTGameplayAbility_Skill::PickCombo_Implementation(AActor* Character, EInputType Input, const FInputActionInstance& InputInstance)
{
	if (Combos.Contains(Input))
	{
		return Combos[Input];
	}

	return nullptr;
}
