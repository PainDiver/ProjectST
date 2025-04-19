// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STWidgetController.h"
#include "GAS/STAttributeSet.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "STWidgetController_HUDBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChanged, const FComboInfoCache&, Info);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, float, OldValue, float, NewValue, float, MaxValue, AActor*, Instigator);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, OldValue, float, NewValue, float, MaxValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommitted, UGameplayAbility*, GA);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityCommitFailed, const UGameplayAbility*, GA, const FGameplayTagContainer&, ExplainingTag);


UCLASS(Blueprintable, BlueprintType)
class PROJECTST_API USTWidgetController_HUD : public USTWidgetController
{
	GENERATED_BODY()
public:
	USTWidgetController_HUD();

	virtual void BeginDestroy()override;

	virtual void OnWidgetControllerSet(APlayerController* Owner)override;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAbilitySystemComponent* ASC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USTComboManagingComponent* ComboComponent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USTStateHandlingComponent* StateComponent;


// Skills///////////////////////
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChanged, const FComboInfoCache&, Info);
	UFUNCTION()
	void OnSkillChanged(const FComboInfoCache& Info);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnSkillChanged OnSkillChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityCommitFailed, const UGameplayAbility*, GA, const FGameplayTagContainer&, ExplainingTag);
	UFUNCTION()
	void OnAbilityCommitFailed(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAbilityCommitFailed OnAbilityCommitFailedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommitted, UGameplayAbility*, GA);
	UFUNCTION()
	void OnAbilityCommitted(UGameplayAbility* GA);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAbilityCommitted OnAbilityCommitDelegate;
////////////////////////////////////////


///////////////////// Stat//////
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnyStatChanged,const FGameplayAttribute&,Attribute ,float, Value);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAnyStatChanged OnAnyStatChangedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, float, OldValue, float, NewValue, float, MaxValue, AActor*, Instigator);
	void OnHealthChanged(const FOnAttributeChangeData& Changed);	
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnHealthChanged OnHealthChangedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, OldValue, float, NewValue, float, MaxValue);
	void OnStaminaChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnStaminaChanged OnStaminaChangedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxHealthChanged, float, OldValue, float, NewValue);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnMaxHealthChanged OnMaxHealthChangedDelegate;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaxStaminaChanged, float, OldValue, float, NewValue);
	void OnMaxStaminaChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnMaxStaminaChanged OnMaxStaminaChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDefenseChanged, float, OldValue, float, NewValue);
	void OnDefenseChanged(const FOnAttributeChangeData& Changed);	
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnDefenseChanged OnDefenseChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCriticalChanceChanged, float, OldValue, float, NewValue);
	void OnCriticalChanceChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnCriticalChanceChanged OnCriticalChanceChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCriticalDamanageChanged, float, OldValue, float, NewValue);
	void OnCriticalDamageChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnCriticalDamanageChanged OnCriticalDamageChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthRegenChanged, float, OldValue, float, NewValue);
	void OnHealthRegenChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnHealthRegenChanged OnHealthRegenChangedDelegate;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaRegenChanged, float, OldValue, float, NewValue);
	void OnStaminaRegenChanged(const FOnAttributeChangeData& Changed);
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnStaminaRegenChanged OnStaminaRegenChangedDelegate;

///////////////////////////////////////

	TMap<FGameplayAttribute,FDelegateHandle> BoundDatas;

	FDelegateHandle CommitDelegateHandle;
	FDelegateHandle CommitFailedDelegateHandle;


};
