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
		return NewObject<UComboContext_Default>(Outer);
	case EComboContextState::JUMPING:
		return NewObject<UComboContext_Jumping>(Outer);
	case EComboContextState::ON_HIT:
		return NewObject<UComboContext_OnHit>(Outer);
	default:
		return nullptr;
	}

}



void UComboContext_Default::ProcessCombo(UAbilitySystemComponent* OwnerASC,USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{
	if (ASTCharacterBase* Character = Cast<ASTCharacterBase>(OwnerASC->GetAvatarActor()))
	{
		// ComboCotext로 다음 콤보 탐색 없을 시,
		if (ComboManaingComp->SetPendingCombo({ InputType,InputInstance }, ComboManaingComp->GetPendingComboTagRef()))
		{
			// UE_LOG(LogTemp, Warning, TEXT("Best Combo Chosen as %s"), ComboManaingComp->GetPendingComboTagRef().GetTagName());
		}
		else if(ComboManaingComp->GetRootComboSet().Contains(InputType))
		{
			TSubclassOf<UGameplayAbility> AbilityToPlay = ComboManaingComp->GetRootComboSet()[InputType];
			OwnerASC->TryActivateAbilityByClass(AbilityToPlay);
		}
	}
}

void UComboContext_Jumping::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{

}

void UComboContext_OnHit::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)
{

}
