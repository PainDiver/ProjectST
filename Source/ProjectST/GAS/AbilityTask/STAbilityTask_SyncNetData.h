// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetData/TargetData.h"

#include "STAbilityTask_SyncNetData.generated.h"


UENUM(BlueprintType)
enum class ENetSyncDirection: uint8
{
	ServerGiveData,
	ClientGiveData
};

UCLASS()
class PROJECTST_API USTAbilityTask_SyncNetData : public UAbilityTask
{
	GENERATED_BODY()	
public:	
	USTAbilityTask_SyncNetData(const FObjectInitializer& ObjectInitializer);
	virtual void Activate()override;
	

	UFUNCTION()
	void OnSignalCallBack(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag);
	
	virtual void OnReceivedData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag) {};
		
	virtual FGameplayAbilityTargetDataHandle MakeTargetData(FGameplayTag& OutTag) { return FGameplayAbilityTargetDataHandle(); };
protected:
	ENetSyncDirection SyncDirection;
};

UCLASS()
class PROJECTST_API USTAbilityTask_SyncNetData_Vector : public USTAbilityTask_SyncNetData
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSyncNetData_Vector, const TArray<FVector>&, Vector);

	UPROPERTY(BlueprintAssignable)
	FSyncNetData_Vector OnSync;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USTAbilityTask_SyncNetData_Vector* SyncNetData_Vector(
		UGameplayAbility* OwningAbility,
		ENetSyncDirection SyncDirection,
		TArray<FVector> Data);
	virtual FGameplayAbilityTargetDataHandle MakeTargetData(FGameplayTag& OutTag);
	virtual void OnReceivedData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag)override;

protected:
	UPROPERTY()
	TArray<FVector> Data;
};


UCLASS()
class PROJECTST_API USTAbilityTask_SyncNetData_GenericData : public USTAbilityTask_SyncNetData
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSyncNetData_GenericData, const FSTGenericGameplayAbilityTargetDataParams&,Data);

	UPROPERTY(BlueprintAssignable)
	FSyncNetData_GenericData OnSync;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static USTAbilityTask_SyncNetData_GenericData* SyncNetData_GenericData(
		UGameplayAbility* OwningAbility,
		ENetSyncDirection SyncDirection,
		FSTGenericGameplayAbilityTargetDataParams GenericParams);

	virtual FGameplayAbilityTargetDataHandle MakeTargetData(FGameplayTag& OutTag);
	virtual void OnReceivedData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag)override;

protected:
	UPROPERTY()
	FSTGenericGameplayAbilityTargetDataParams Data;
};
