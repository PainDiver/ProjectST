// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "HAL/Event.h"
#include "Game/STNativeGameplayTag.h"
#include "STEventManager.generated.h"

/**
 * 
 */

 //Event
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Event_CharacterPrepared)

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
		if(Events.IsValidIndex(Index))
			Events.RemoveAt(Index);
		if(BoundData.IsValidIndex(Index))
			BoundData.RemoveAt(Index);		
		if(bShouldFireOnce.IsValidIndex(Index))
			bShouldFireOnce.RemoveAt(Index);
	}

	UPROPERTY()
	TArray<FSTEventDelegate> Events;

	UPROPERTY()
	TArray<UObject*> BoundData;

	UPROPERTY()
	TArray<bool> bShouldFireOnce;
};

USTRUCT()
struct FSTEventDelegateBucket_Subject
{
	GENERATED_BODY()

	FSTEventDelegateBucket_Subject() {}

	void AddEvent(const FGameplayTag& Tag, FSTEventDelegate&& Delegate, UObject* Data, bool bFireOnce)
	{		
		if (Events.Contains(Tag))
		{
			FSTEventDelegateBucket& Bucket = Events[Tag];
			Bucket.AddEvent(MoveTemp(Delegate), Data, bFireOnce);
		}
		else
		{
			FSTEventDelegateBucket Bucket;
			Bucket.AddEvent(MoveTemp(Delegate), Data, bFireOnce);
			Events.Add(Tag,Bucket);
		}
	}

	void RemoveEvent(const FGameplayTag& Tag,int32 Index)
	{
		if (Events.Contains(Tag))
		{
			FSTEventDelegateBucket& Bucket = Events[Tag];
			Bucket.RemoveEvent(Index);
		}
	}

	UPROPERTY()
	TMap<FGameplayTag, FSTEventDelegateBucket> Events;
};

UCLASS()
class PROJECTST_API USTEventManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	static USTEventManager* GetEventManager()
	{
		if (GEngine == nullptr)
			return nullptr;

		UGameInstance* GameInstance = GEngine->GetCurrentPlayWorld()->GetGameInstance();
		if (GameInstance == nullptr)
			return nullptr;

		return GameInstance->GetSubsystem<USTEventManager>();

	}


	virtual void Initialize(FSubsystemCollectionBase& Collection)override;

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Delegate"))
	void RegisterEvent(FGameplayTag Tag, FSTEventDelegate Delegate,UObject* Data,bool bFireOnce);

	UFUNCTION(BlueprintCallable)
	void FireEvent(FGameplayTag Tag,UObject* Data);


	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "Delegate"))
	void RegisterEvent_Subject(UObject* Subject,FGameplayTag Tag, FSTEventDelegate Delegate, UObject* Data, bool bFireOnce);

	UFUNCTION(BlueprintCallable)
	void FireEvent_Subject(UObject* Subject, FGameplayTag Tag, UObject* Data);


	template<typename FT>
	FT AsyncTaskAndWait(FName AsyncTaskName,EAsyncExecution ThreadType,TFunction<void()>&& AsyncAction, TFunction<FT()>&& GetAction)
	{
		FEvent* TaskCompletedEvent = FPlatformProcess::CreateSynchEvent(false);
		AsyncConditions.Add(AsyncTaskName,TaskCompletedEvent);
		TFuture<FT> Data = Async(
			EAsyncExecution::TaskGraph,
			[TaskCompletedEvent, 
			ActionCallback = MoveTemp(AsyncAction),
			GetCallBack = MoveTemp(GetAction)
			]()
			{
				ActionCallback();
				TaskCompletedEvent->Wait();
				TaskCompletedEvent->Reset();
				FT Result = GetCallBack();
				return Result;
			});

		return Data.Get();
	}

	void NotifyAsyncDone(FName AsyncTaskName);

private:

	TMap<FName, FEvent*> AsyncConditions;

	UPROPERTY()
	TMap<FGameplayTag, FSTEventDelegateBucket> Event_NoSubject;

	UPROPERTY()
	TMap<UObject*,FSTEventDelegateBucket_Subject> Event_Subject;
};
