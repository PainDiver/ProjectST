// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/AbilityTask/STAbilityTask_SyncNetData.h"
#include "AbilitySystemComponent.h"
#include "GAS/STAbilitySystemComponent.h"


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
				FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
				
				OnSignalCallBack(TargetDataHandle, Tag);

				USTAbilitySystemComponent* ASC = Cast<USTAbilitySystemComponent>(AbilitySystemComponent.Get());
				ASC->ClientSetReplicatedTargetData(
					GetAbilitySpecHandle(),
					ActivationPredictionKey,
					TargetDataHandle,
					FGameplayTag::EmptyTag,
					AbilitySystemComponent->ScopedPredictionKey);

			}
			else
			{
				FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
				FAbilityTargetDataSetDelegate& Delegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), ActivationPredictionKey);
				Delegate.AddUObject(this,&USTAbilityTask_SyncNetData::OnSignalCallBack);
				AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), ActivationPredictionKey);
			}
		}
		else
		{
			if (AbilitySystemComponent->IsOwnerActorAuthoritative())
			{
				FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
				FAbilityTargetDataSetDelegate& Delegate = AbilitySystemComponent->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), ActivationPredictionKey);
				Delegate.AddUObject(this, &USTAbilityTask_SyncNetData::OnSignalCallBack);
				AbilitySystemComponent->CallReplicatedTargetDataDelegatesIfSet(GetAbilitySpecHandle(), ActivationPredictionKey);
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
	OnReceivedData(TargetDataHandle,Tag);

	AbilitySystemComponent->ConsumeClientReplicatedTargetData(
		GetAbilitySpecHandle(),
		AbilitySystemComponent->ScopedPredictionKey);
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



USTAbilityTask_SyncNetData_GenericData* USTAbilityTask_SyncNetData_GenericData::SyncNetData_GenericData(UGameplayAbility* OwningAbility, ENetSyncDirection SyncDirection, FSTGenericGameplayAbilityTargetDataParams GenericParams)
{
	USTAbilityTask_SyncNetData_GenericData* Task = NewAbilityTask<USTAbilityTask_SyncNetData_GenericData>(OwningAbility);
	Task->SyncDirection = SyncDirection;
	Task->Data = MoveTemp(GenericParams);
	return Task;
}

FGameplayAbilityTargetDataHandle USTAbilityTask_SyncNetData_GenericData::MakeTargetData(FGameplayTag& OutTag)
{
	FGameplayAbilityTargetDataHandle Handle;
	FSTGenericGameplayAbilityTargetData* TargetData = new FSTGenericGameplayAbilityTargetData();	
	TargetData->SetParam(MoveTemp(Data));

	Handle.Add(TargetData);
	return Handle;
}

void USTAbilityTask_SyncNetData_GenericData::OnReceivedData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, FGameplayTag Tag)
{
	if (TargetDataHandle.Data.IsValidIndex(0))
	{
		const FSTGenericGameplayAbilityTargetData* Casted = static_cast<const FSTGenericGameplayAbilityTargetData*>(TargetDataHandle.Get(0));
		FSTGenericGameplayAbilityTargetDataParams Param;
		Param.TagContainer = Casted->TagContainer;
		Param.Objects = Casted->Objects;
		Param.Vectors = Casted->Vectors;
		Param.Floats = Casted->Floats;
		Param.Enums = Casted->Enums;
		Param.Ints = Casted->Ints;
		Param.Names = Casted->Names;

		OnSync.Broadcast(Param);
	}
}
