// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "STStateInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTST_API ISTStateInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	int32 GetCharacterID();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable,meta = (GameplayTagFilter = "State"))
	bool AddState(const FGameplayTag& Tag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	void RemoveState(const FGameplayTag& Tag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	bool AddState_Replication(const FGameplayTag& Tag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	void RemoveState_Replication(const FGameplayTag& Tag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	void OnDead(AActor* Killer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	void OnKill(AActor* Killed);


	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FGameplayTagContainer GetStates();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	bool HasState(const FGameplayTag& Tag);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, meta = (GameplayTagFilter = "State"))
	bool IsImmortalState();


	UFUNCTION(BlueprintNativeEvent, meta = (GameplayTagFilter = "State"))
	void OnAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue);

};
