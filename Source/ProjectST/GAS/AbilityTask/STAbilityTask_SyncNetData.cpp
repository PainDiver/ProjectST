// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/STAbilityTask_SyncNetData.h"
#include "AbilitySystemComponent.h"




USTAbilityTask_SyncNetData::USTAbilityTask_SyncNetData(const FObjectInitializer& ObjectInitializer)
	: UAbilityTask(ObjectInitializer)
{
}


void USTAbilityTask_SyncNetData::Activate()
{
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get(), IsPredictingClient());
	
	if (AbilitySystemComponent->GetNetMode() == ENetMode::NM_Standalone)
	{
		FGameplayTag Tag;
		FGameplayAbilityTargetDataHandle TargetDataHandle = MakeTargetData(Tag);
		OnSignalCallBack(TargetDataHandle, Tag);
	}
	else
	{
		if (SyncDirection == ENetSyncDirection::ServerGiveData)
		{
			if (AbilitySystemComponent->IsOwnerActorAuthoritative())
			{
				FGameplayTag Tag;
				FGameplayAbilityTargetDataHandle TargetDataHandle = MakeTargetData(Tag);

				AbilitySystemComponent->ServerSetReplicatedTargetData(
					GetAbilitySpecHandle(),
					GetActivationPredictionKey(),
					TargetDataHandle,
					FGameplayTag::EmptyTag,
					AbilitySystemComponent->ScopedPredictionKey);

				OnSignalCallBack(TargetDataHandle, Tag);
			}
			else
			{
				FAbilityTargetDataSetDelegate& Delegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(),AbilitySystemComponent->ScopedPredictionKey);
				Delegate.AddUObject(this,&USTAbilityTask_SyncNetData::OnSignalCallBack);
				AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), AbilitySystemComponent->ScopedPredictionKey);
			}
		}
		else
		{
			if (AbilitySystemComponent->IsOwnerActorAuthoritative())
			{
				FAbilityTargetDataSetDelegate& Delegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), AbilitySystemComponent->ScopedPredictionKey);
				Delegate.AddUObject(this, &USTAbilityTask_SyncNetData::OnSignalCallBack);
				AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), AbilitySystemComponent->ScopedPredictionKey);
			}
			else
			{
				FGameplayTag Tag;
				FGameplayAbilityTargetDataHandle TargetDataHandle = MakeTargetData(Tag);
				AbilitySystemComponent->ServerSetReplicatedTargetData(
					GetAbilitySpecHandle(),
					GetActivationPredictionKey(),
					TargetDataHandle,
					FGameplayTag::EmptyTag,
					AbilitySystemComponent->ScopedPredictionKey);

				OnSignalCallBack(TargetDataHandle, Tag);
			}
		}
	}
}

void USTAbilityTask_SyncNetData::OnSignalCallBack(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetAbilitySpecHandle(),
		AbilitySystemComponent->ScopedPredictionKey);

	OnReceivedData(TargetDataHandle,Tag);
}



USTAbilityTask_SyncNetData_Vector* USTAbilityTask_SyncNetData_Vector::SyncNetData_Vector(UGameplayAbility* OwningAbility, ENetSyncDirection SyncDirection, TArray<FVector> Data)
{
	USTAbilityTask_SyncNetData_Vector* Task = NewAbilityTask<USTAbilityTask_SyncNetData_Vector>(OwningAbility);
	Task->SyncDirection = SyncDirection;
	Task->Data = Data;
	return Task;
}
FGameplayAbilityTargetDataHandle USTAbilityTask_SyncNetData_Vector::MakeTargetData(FGameplayTag& OutTag)
{
	FGameplayAbilityTargetDataHandle Handle;
	FGameplayAbilityTargetData_Vector* TargetData = new FGameplayAbilityTargetData_Vector();
	TargetData->Data = Data;
	Handle.Add(TargetData);
	return Handle;
}
void USTAbilityTask_SyncNetData_Vector::OnReceivedData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag)
{
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		TSharedPtr<FGameplayAbilityTargetData_Vector> Casted = StaticCastSharedPtr<FGameplayAbilityTargetData_Vector>(TargetDataHandle.Data[0]);
		OnSync.Broadcast(Casted->Data);
	}
}
