// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STWidgetController.h"
#include "GAS/STAttributeSet.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "STWidgetController_HUDBase.generated.h"


UCLASS(Blueprintable, BlueprintType)
class PROJECTST_API USTWidgetController_HUD : public USTWidgetController
{
	GENERATED_BODY()
public:

	virtual void BeginDestroy()override;

	virtual void OnWidgetControllerSet(APlayerController* Owner)override;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UAbilitySystemComponent* ASC;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class USTComboManagingComponent* ComboComponent;

	UFUNCTION()
	void OnSkillChanged(const FComboInfoCache& Info);

	void OnHealthChanged(const FOnAttributeChangeData& Changed);	

	void OnStaminaChanged(const FOnAttributeChangeData& Changed);
	
	UFUNCTION()
	void OnAbilityCommitted(UGameplayAbility* GA);

	UFUNCTION()
	void OnAbilityCommitFailed(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillChanged, const FComboInfoCache&, Info);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, float, OldValue, float, NewValue,float,MaxValue ,AActor*, Instigator);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStaminaChanged, float, OldValue, float, NewValue, float,MaxValue);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommitted, UGameplayAbility*, GA);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityCommitFailed, const UGameplayAbility*, GA, const FGameplayTagContainer&, ExplainingTag);


	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnSkillChanged OnSkillChangedDelegate;

	UPROPERTY(BlueprintAssignable,BlueprintReadWrite)
	FOnHealthChanged OnHealthChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnStaminaChanged OnStaminaChangedDelegate;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAbilityCommitted OnAbilityCommitDelegate;
	
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite)
	FOnAbilityCommitFailed OnAbilityCommitFailedDelegate;

	FDelegateHandle HealthDelegateHandle;
	FDelegateHandle StaminaDelegateHandle;
	FDelegateHandle CommitDelegateHandle;
	FDelegateHandle CommitFailedDelegateHandle;
};
