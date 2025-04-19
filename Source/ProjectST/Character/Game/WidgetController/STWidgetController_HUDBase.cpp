// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Game/WidgetController/STWidgetController_HUDBase.h"
#include "AbilitySystemComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectExtension.h"
#include "Character/Component/ManagedStates/STManagedState_LockOn.h"
#include "Character/Component/STStateHandlingComponent.h"

USTWidgetController_HUD::USTWidgetController_HUD()
{
	ControllerType = EWidgetControllerType::HUDBaseController;
}

void USTWidgetController_HUD::BeginDestroy()
{
	Super::BeginDestroy();

	if (ASC == nullptr)
	{
		return;
	}

	ComboComponent->OnComboInfoChanged.RemoveDynamic(this, &ThisClass::OnSkillChanged);
	ASC->AbilityCommittedCallbacks.Remove(CommitDelegateHandle);
	ASC->AbilityFailedCallbacks.Remove(CommitFailedDelegateHandle);

	for (TPair<FGameplayAttribute, FDelegateHandle>& BoundDelegate : BoundDatas)
	{
		FOnGameplayAttributeValueChange& Delegate = ASC->GetGameplayAttributeValueChangeDelegate(BoundDelegate.Key);
		Delegate.Remove(BoundDelegate.Value);
	}
	
}

void USTWidgetController_HUD::OnSkillChanged(const FComboInfoCache& Info)
{
	OnSkillChangedDelegate.Broadcast(Info);
}

void USTWidgetController_HUD::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	bool bFound = false;
	float MaxHealth = ASC->GetGameplayAttributeValue(USTAttributeSet::GetMaxHealthAttribute(),bFound);
	OnHealthChangedDelegate.Broadcast(Data.OldValue, Data.NewValue,MaxHealth,Data.GEModData ? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr);
}

void USTWidgetController_HUD::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	bool bFound = false;
	float MaxStamina = ASC->GetGameplayAttributeValue(USTAttributeSet::GetMaxStaminaAttribute(),bFound);
	OnStaminaChangedDelegate.Broadcast(Data.OldValue, Data.NewValue,MaxStamina);
}

void USTWidgetController_HUD::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnMaxHealthChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}

void USTWidgetController_HUD::OnMaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnMaxStaminaChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}

void USTWidgetController_HUD::OnDefenseChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnDefenseChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}

void USTWidgetController_HUD::OnCriticalChanceChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnCriticalChanceChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}

void USTWidgetController_HUD::OnCriticalDamageChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnCriticalDamageChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}

void USTWidgetController_HUD::OnHealthRegenChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnHealthRegenChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
}


void USTWidgetController_HUD::OnStaminaRegenChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;
	OnAnyStatChangedDelegate.Broadcast(Data.Attribute, Data.NewValue);

	OnStaminaRegenChangedDelegate.Broadcast(Data.OldValue, Data.NewValue);
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
	ASC->AbilityCommittedCallbacks.Add(CommitDelegate);
	CommitDelegateHandle = CommitDelegate.GetHandle();

	auto CommitFailDelegate = FAbilityFailedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitFailed);
	ASC->AbilityFailedCallbacks.Add(CommitFailDelegate);
	CommitFailedDelegateHandle = CommitFailDelegate.GetHandle();

	FOnGameplayAttributeValueChange& OnHealthChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
	auto HealthDelegate =  FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnHealthChanged);
	OnHealthChanged.Add(HealthDelegate);
	BoundDatas.Add(USTAttributeSet::GetCurrentHealthAttribute(), HealthDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnMaxHealthChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetMaxHealthAttribute());
	auto MaxHealthDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnMaxHealthChanged);
	OnMaxHealthChanged.Add(MaxHealthDelegate);
	BoundDatas.Add(USTAttributeSet::GetMaxHealthAttribute(), MaxHealthDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnStaminaChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
	auto StaminaDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnStaminaChanged);
	OnStaminaChanged.Add(StaminaDelegate);
	BoundDatas.Add(USTAttributeSet::GetCurrentStaminaAttribute(), StaminaDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnMaxStaminaChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetMaxStaminaAttribute());
	auto MaxStaminaDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnMaxStaminaChanged);
	OnMaxStaminaChanged.Add(MaxStaminaDelegate);
	BoundDatas.Add(USTAttributeSet::GetMaxStaminaAttribute(), MaxStaminaDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnDefenseChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetDefenseAttribute());
	auto DefenseDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnDefenseChanged);
	OnDefenseChanged.Add(DefenseDelegate);
	BoundDatas.Add(USTAttributeSet::GetDefenseAttribute(), DefenseDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnCriticalChanceChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCriticalChanceAttribute());
	auto CriticalChanceDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnCriticalChanceChanged);
	OnCriticalChanceChanged.Add(CriticalChanceDelegate);
	BoundDatas.Add(USTAttributeSet::GetCriticalChanceAttribute(), CriticalChanceDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnCriticalDamageChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCriticalDamageAttribute());
	auto CriticalDamageDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnCriticalDamageChanged);
	OnCriticalDamageChanged.Add(CriticalDamageDelegate);
	BoundDatas.Add(USTAttributeSet::GetCriticalDamageAttribute(), CriticalDamageDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnHealthRegenChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetHealthRegenAttribute());
	auto HealthRegenDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnHealthRegenChanged);
	OnHealthRegenChanged.Add(HealthRegenDelegate);
	BoundDatas.Add(USTAttributeSet::GetHealthRegenAttribute(), HealthRegenDelegate.GetHandle());

	FOnGameplayAttributeValueChange& OnStaminaRegenChanged = ASC->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetStaminaRegenAttribute());
	auto StaminaRegenDelegate = FOnGameplayAttributeValueChange::FDelegate::CreateUObject(this, &USTWidgetController_HUD::OnStaminaRegenChanged);
	OnStaminaRegenChanged.Add(StaminaRegenDelegate);
	BoundDatas.Add(USTAttributeSet::GetStaminaRegenAttribute(), StaminaRegenDelegate.GetHandle());

	BoundDatas.Shrink();
}
