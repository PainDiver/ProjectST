// Fill out your copyright notice in the Description page of Project Settings.


#include "STEventManager.h"

UE_DEFINE_GAMEPLAY_TAG(Event_CharacterPrepared, "Event.CharacterPrepared");


void USTEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Event_NoSubject.Reserve(30);
}

void USTEventManager::RegisterEvent(FGameplayTag Tag, FSTEventDelegate Delegate, UObject* Data,bool bFireOnce)
{
	if (Event_NoSubject.Contains(Tag))
	{
		Event_NoSubject[Tag].AddEvent(MoveTemp(Delegate), Data, bFireOnce);
	}
	else
	{
		FSTEventDelegateBucket NewBucket = FSTEventDelegateBucket();
		NewBucket.AddEvent(MoveTemp(Delegate),Data,bFireOnce);
		Event_NoSubject.Add(Tag,NewBucket);
	}
}

void USTEventManager::FireEvent(FGameplayTag Tag, UObject* Data)
{
	if (Event_NoSubject.Contains(Tag))
	{
		TArray<int32> RemovedEventIndices;
		for (int i=0;i< Event_NoSubject[Tag].Events.Num();i++ )
		{
			UObject* BoundData = Event_NoSubject[Tag].BoundData[i];
			UObject* ChosenData = BoundData?BoundData:Data;
			Event_NoSubject[Tag].Events[i].ExecuteIfBound(ChosenData);

			if (Event_NoSubject[Tag].bShouldFireOnce[i])
			{
				RemovedEventIndices.Add(i);
			}
		}	

		for (int32 IndexToRemove : RemovedEventIndices)
		{		
			Event_NoSubject[Tag].RemoveEvent(IndexToRemove);
		}
	}
}

void USTEventManager::RegisterEvent_Subject(UObject* Subject, FGameplayTag Tag, FSTEventDelegate Delegate, UObject* Data, bool bFireOnce)
{
	if (Event_Subject.Contains(Subject))
	{
		FSTEventDelegateBucket_Subject& Bucket = Event_Subject[Subject];
		Bucket.AddEvent(Tag, MoveTemp(Delegate), Data, bFireOnce);
	}
	else
	{
		FSTEventDelegateBucket_Subject NewBucket = FSTEventDelegateBucket_Subject();
		NewBucket.AddEvent(Tag,MoveTemp(Delegate), Data, bFireOnce);
		Event_Subject.Add(Subject, NewBucket);
	}
}

void USTEventManager::FireEvent_Subject(UObject* Subject, FGameplayTag Tag, UObject* Data)
{
	if (Event_Subject.Contains(Subject))
	{
		if (Event_Subject[Subject].Events.Contains(Tag))
		{
			FSTEventDelegateBucket& Bucket = Event_Subject[Subject].Events[Tag];

			TArray<int32> RemovedEventIndices;
			for (int i = 0; i < Bucket.Events.Num(); i++)
			{
				UObject* BoundData = Bucket.BoundData[i];
				UObject* ChosenData = BoundData ? BoundData : Data;
				Bucket.Events[i].ExecuteIfBound(ChosenData);

				if (Bucket.bShouldFireOnce[i])
				{
					RemovedEventIndices.Add(i);
				}
			}

			for (int32 IndexToRemove : RemovedEventIndices)
			{
				Bucket.RemoveEvent(IndexToRemove);
			}
		}
	}
}

void USTEventManager::NotifyAsyncDone(FName AsyncTaskName)
{
	if (AsyncConditions.Contains(AsyncTaskName))
	{
		AsyncConditions[AsyncTaskName]->Trigger();
	}
}


