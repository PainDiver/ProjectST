// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "STEventManager.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FSTEventDelegate, UObject*, Data);

USTRUCT()
struct FSTEventDelegateBucket
{
	GENERATED_BODY()

	FSTEventDelegateBucket()
	{
		Events.Reserve(5);
		BoundData.Reserve(5);
		bShouldFireOnce.Reserve(5);
	}

	void AddEvent(FSTEventDelegate&& Delegate, UObject* Data, bool bFireOnce)
	{
		Events.Add(MoveTemp(Delegate));
		BoundData.Add(Data);
		bShouldFireOnce.Add(bFireOnce);
	}

	void RemoveEvent(int32 Index)
	{
		Events.RemoveAt(Index);
		BoundData.RemoveAt(Index);
		bShouldFireOnce.RemoveAt(Index);
	}

	UPROPERTY()
	TArray<FSTEventDelegate> Events;

	UPROPERTY()
	TArray<UObject*> BoundData;

	UPROPERTY()
	TArray<bool> bShouldFireOnce;

};

UCLASS()
class PROJECTST_API USTEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection)override;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Delegate"))
	void RegisterEvent(FGameplayTag Tag, FSTEventDelegate Delegate,UObject* Data,bool bFireOnce);

	UFUNCTION(BlueprintCallable)
	void FireEvent(FGameplayTag Tag,UObject* Data);

private:

	UPROPERTY()
	TMap<FGameplayTag, FSTEventDelegateBucket> STEvents;

};
