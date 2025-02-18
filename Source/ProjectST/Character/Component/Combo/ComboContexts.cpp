// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/Combo/ComboContexts.h"
#include "GAS/GA/STGameplayAbility_Skill.h"
#include "AbilitySystemComponent.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/Combo/STComboManagingComponent.h"


UComboContext* UComboContext::CreateContext(UObject* Outer, EComboContextState State)
{
	switch (State)
	{
	case EComboContextState::DEFAULT:
		return NewObject<CombatStateClass<EComboContextState::DEFAULT>::Type>(Outer);
	case EComboContextState::JUMPING:
		return NewObject<CombatStateClass<EComboContextState::JUMPING>::Type>(Outer);
	case EComboContextState::ON_HIT:
		return NewObject<CombatStateClass<EComboContextState::ON_HIT>::Type>(Outer);
	default:
		return nullptr;
	}

}



void UComboContext_Default::ProcessCombo(UAbilitySystemComponent* OwnerASC,USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{
	if (ASTCharacterBase* Character = Cast<ASTCharacterBase>(OwnerASC->GetAvatarActor()))
	{
		TSubclassOf<UGameplayAbility> AbilityToPlay;
		if (USTGameplayAbility_Skill* CurrentSkillAbility = Cast<USTGameplayAbility_Skill>(OwnerASC->GetAnimatingAbility()))
		{
			AbilityToPlay = CurrentSkillAbility->PickCombo(Character, InputType, InputInstance);
		}		
		
		if (AbilityToPlay == nullptr && ComboManaingComp->GetRootComboSet().Contains(InputType))
		{
			AbilityToPlay = ComboManaingComp->GetRootComboSet()[InputType];
		}

		OwnerASC->TryActivateAbilityByClass(AbilityToPlay);
	}
}

void UComboContext_Jumping::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{

}

void UComboContext_OnHit::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{

}
