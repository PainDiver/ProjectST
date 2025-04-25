// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "STAbilitySystemComponent.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTST_API USTAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	
	void Initialize(AActor* Owner, AActor* Avatar, uint32 CharacterID, TFunction<void()>&& OnInitCallBack = [](){});

	void AddState(const FGameplayTag& Tag);

	void RemoveState(const FGameplayTag& Tag);

	void AddState_Replication(const FGameplayTag& Tag);

	void RemoveState_Replication(const FGameplayTag& Tag);

	bool HasState(const FGameplayTag& Tag);

	void ResolveStateCollapse(const FGameplayTag& NewTag);

	FGameplayTagContainer GetStates();

	FGameplayAbilityReplicatedDataContainer& GetReplicatedAbilityTargetDataMap();

	UFUNCTION(Client, Reliable)
	void ClientSetReplicatedTargetData(const FGameplayAbilitySpecHandle& AbilityHandle, FPredictionKey AbilityOriginalPredictionKey, const FGameplayAbilityTargetDataHandle& ReplicatedTargetDataHandle, FGameplayTag ApplicationTag, FPredictionKey CurrentPredictionKey);


	UFUNCTION(BlueprintCallable)
	void RemoveGameplayEffectByClass(TSubclassOf<UGameplayEffect> Effect);

};
