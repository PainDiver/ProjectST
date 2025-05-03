// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/Combo/ComboContexts.h"
#include "GAS/GA/STGameplayAbility_Skill.h"
#include "AbilitySystemComponent.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Game/STNativeGameplayTag.h"

void UComboContext::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{
	bool Res = false;

	// ComboCotext로 다음 콤보 탐색 없을 시,
	if (ComboManaingComp->SetPendingCombo(FInputDetail{ InputType,InputInstance.GetTriggerEvent(),InputInstance}))
	{
		Res = true;
	}
	else if (ComboManaingComp->GetRootComboSet(ComboContextState).Contains(InputType))
	{
		TSubclassOf<UGameplayAbility> AbilityToPlay = ComboManaingComp->GetRootComboSet(ComboContextState)[InputType];
		USTGameplayAbility* AbilityInstance = nullptr;
		bool bIsActive = false;
		if (FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromClass(AbilityToPlay))
		{
			AbilityInstance = Cast<USTGameplayAbility>(Spec->Ability);
			bIsActive = (Spec->ActiveCount > 0) || OwnerASC->IsAnimatingAbility(AbilityInstance);
		}

		bool ExeptionalCondition = false;
		bool bShouldActivateAnyway = false;
		if (AbilityInstance)
		{
			ExeptionalCondition = (AbilityInstance && bIsActive && AbilityInstance->CanRetriggerOnSameAbility());
			bShouldActivateAnyway = AbilityInstance->CanActivateAbilityWhenever();
		}

		if (!OwnerASC->GetAnimatingAbility() || ExeptionalCondition || bShouldActivateAnyway)
		{
			UComboInputData* ComboInput = NewObject<UComboInputData>();
			ComboInput->InputType = InputType;
			ComboInput->InputInstance = InputInstance;
			TSharedPtr<FGameplayEventData> GameplayEventData = MakeShared<FGameplayEventData>();
			GameplayEventData->OptionalObject2 = ComboInput;
			OwnerASC->TryActivateAbilityByClass(AbilityToPlay);
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerASC->GetAvatarActor(), Input_InputInstance, *GameplayEventData);
			Res = true;
		}
	}

	if (CallBack)
		CallBack(Res);
}

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



void UComboContext_Default::ProcessCombo(UAbilitySystemComponent* OwnerASC,USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{	
	UComboContext::ProcessCombo(OwnerASC, ComboManaingComp, InputType, InputInstance, MoveTemp(CallBack));
}

void UComboContext_Jumping::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{
	UComboContext::ProcessCombo(OwnerASC, ComboManaingComp, InputType, InputInstance, MoveTemp(CallBack));
}

void UComboContext_OnHit::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{
	UComboContext::ProcessCombo(OwnerASC, ComboManaingComp, InputType, InputInstance, MoveTemp(CallBack));
}
