// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/Combo/ComboContexts.h"
#include "GAS/GA/STGameplayAbility_Skill.h"
#include "AbilitySystemComponent.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Game/STNativeGameplayTag.h"

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
	ASTCharacterBase* Character = Cast<ASTCharacterBase>(OwnerASC->GetAvatarActor());
	if (Character == nullptr)
		return;
	bool Res = false;

	// ComboCotext로 다음 콤보 탐색 없을 시,
	if (ComboManaingComp->SetPendingCombo({ InputType,InputInstance.GetTriggerEvent()}, ComboManaingComp->GetPendingComboTagRef()))
	{
		Res = true;
	}
	else if(ComboManaingComp->GetRootComboSet(EComboContextState::DEFAULT).Contains(InputType) && (!OwnerASC->GetAnimatingAbility()))
	{			
		TSubclassOf<UGameplayAbility> AbilityToPlay = ComboManaingComp->GetRootComboSet(EComboContextState::DEFAULT)[InputType];

		FScopedPredictionWindow Window(OwnerASC,true);

		UComboInputData* ComboInput = NewObject<UComboInputData>();				
		ComboInput->InputType = InputType;
		ComboInput->InputInstance = InputInstance;			
		TSharedPtr<FGameplayEventData> GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->OptionalObject2 = ComboInput;
		OwnerASC->TryActivateAbilityByClass(AbilityToPlay);						
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerASC->GetAvatarActor(), Input_InputInstance, *GameplayEventData);
		Res = true;
	}	
	else
	{		
		Res = false;
	}

	if(CallBack)
		CallBack(Res);
}

void UComboContext_Jumping::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{
	ASTCharacterBase* Character = Cast<ASTCharacterBase>(OwnerASC->GetAvatarActor());
	if (Character == nullptr)
		return;

	bool Res = false;
		
	// ComboCotext로 다음 콤보 탐색 없을 시,
	if (ComboManaingComp->SetPendingCombo({ InputType,InputInstance.GetTriggerEvent() }, ComboManaingComp->GetPendingComboTagRef()))
	{
		Res = true;
	}
	else if (ComboManaingComp->GetRootComboSet(EComboContextState::JUMPING).Contains(InputType) && (!OwnerASC->GetAnimatingAbility()))
	{
		TSubclassOf<UGameplayAbility> AbilityToPlay = ComboManaingComp->GetRootComboSet(EComboContextState::JUMPING)[InputType];
		FScopedPredictionWindow Window(OwnerASC, true);

		UComboInputData* ComboInput = NewObject<UComboInputData>();
		ComboInput->InputType = InputType;
		ComboInput->InputInstance = InputInstance;
		TSharedPtr<FGameplayEventData> GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->OptionalObject2 = ComboInput;
		OwnerASC->TryActivateAbilityByClass(AbilityToPlay);
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerASC->GetAvatarActor(), Input_InputInstance, *GameplayEventData);
		Res = true;
	}
	else
	{
		Res = false;
	}

	if (CallBack)
		CallBack(Res);
}

void UComboContext_OnHit::ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{

}
