// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Game/WidgetController/STWidgetController_HUDBase.h"
#include "AbilitySystemComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectExtension.h"
#include "Character/Component/ManagedStates/STManagedState_LockOn.h"
#include "Character/Component/STStateHandlingComponent.h"

void USTWidgetController_HUD::BeginDestroy()
{
	Super::BeginDestroy();

	if (ASC == nullptr)
	{
		return;
	}

	ComboComponent->OnComboInfoChanged.RemoveDynamic(this, &ThisClass::OnSkillChanged);
	FOnGameplayAttributeValueChange& OnHealthChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
	OnHealthChanged.Remove(HealthDelegateHandle);
	FOnGameplayAttributeValueChange& OnStaminaChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
	OnStaminaChanged.Remove(StaminaDelegateHandle);
	ASC->AbilityCommittedCallbacks.Remove(CommitDelegateHandle);
	ASC->AbilityFailedCallbacks.Remove(CommitFailedDelegateHandle);
}

void USTWidgetController_HUD::OnSkillChanged(const FComboInfoCache& Info)
{
	OnSkillChangedDelegate.Broadcast(Info);
}

void USTWidgetController_HUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

	bool bFound = false;
	float MaxHealth = ASC->GetGameplayAttributeValue(USTAttributeSet::GetMaxHealthAttribute(),bFound);
	OnHealthChangedDelegate.Broadcast(Data.OldValue, Data.NewValue,MaxHealth,Data.GEModData ? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr);
}

void USTWidgetController_HUD::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

	bool bFound = false;
	float MaxStamina = ASC->GetGameplayAttributeValue(USTAttributeSet::GetMaxStaminaAttribute(),bFound);
	OnStaminaChangedDelegate.Broadcast(Data.OldValue, Data.NewValue,MaxStamina);
}

void USTWidgetController_HUD::OnAbilityCommitted(UGameplayAbility* GA)
{
	OnAbilityCommitDelegate.Broadcast(GA);
}

void USTWidgetController_HUD::OnAbilityCommitFailed(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag)
{
	OnAbilityCommitFailedDelegate.Broadcast(GA,ExplainingTag);
}


void USTWidgetController_HUD::OnWidgetControllerSet(APlayerController* Owner)
{
	USTWidgetController::OnWidgetControllerSet(Owner);
	
	if (IAbilitySystemInterface* IASC = Cast<IAbilitySystemInterface>(Owner->GetPawn()))
	{
		ASC = IASC->GetAbilitySystemComponent();		
		ComboComponent = Owner->GetPawn()->GetComponentByClass<USTComboManagingComponent>();
		StateComponent = Owner->GetPawn()->GetComponentByClass<USTStateHandlingComponent>();
	}

	if (ASC == nullptr || ComboComponent == nullptr || StateComponent == nullptr)
	{
		return;
	}

	ComboComponent->OnComboInfoChanged.AddDynamic(this,&USTWidgetController_HUD::OnSkillChanged);

	auto CommitDelegate = FGenericAbilityDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitted);
	CommitDelegateHandle = CommitDelegate.GetHandle();
	ASC->AbilityCommittedCallbacks.Add(CommitDelegate);
	
	auto CommitFailDelegate = FAbilityFailedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitFailed);
	CommitFailedDelegateHandle = CommitFailDelegate.GetHandle();
	ASC->AbilityFailedCallbacks.Add(CommitFailDelegate);

	FOnGameplayAttributeValueChange& OnHealthChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
	auto HealthDelegate =  FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnHealthChanged);
	HealthDelegateHandle = HealthDelegate.GetHandle();
	OnHealthChanged.Add(HealthDelegate);

	FOnGameplayAttributeValueChange& OnStaminaChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
	auto StaminaDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnStaminaChanged);
	StaminaDelegateHandle = StaminaDelegate.GetHandle();
	OnStaminaChanged.Add(StaminaDelegate);
	
}
