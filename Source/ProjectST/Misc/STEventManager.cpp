// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STEventManager.h"
#include "STEventManager.h"

void USTEventManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	STEvents.Reserve(30);
}

void USTEventManager::RegisterEvent(FGameplayTag Tag, FSTEventDelegate Delegate, UObject* Data,bool bFireOnce)
{
	if (STEvents.Contains(Tag))
	{
		STEvents[Tag].AddEvent(MoveTemp(Delegate), Data, bFireOnce);
	}
	else
	{
		FSTEventDelegateBucket NewBucket = FSTEventDelegateBucket();
		NewBucket.AddEvent(MoveTemp(Delegate),Data,bFireOnce);
		STEvents.Add(Tag,NewBucket);
	}
}

void USTEventManager::FireEvent(FGameplayTag Tag, UObject* Data)
{
	if (STEvents.Contains(Tag))
	{
		TArray<int32> RemovedEventIndices;
		for (int i=0;i< STEvents[Tag].Events.Num();i++ )
		{
			UObject* BoundData = STEvents[Tag].BoundData[i];
			UObject* ChosenData = BoundData?BoundData:Data;
			STEvents[Tag].Events[i].ExecuteIfBound(ChosenData);

			if (STEvents[Tag].bShouldFireOnce[i])
			{
				RemovedEventIndices.Add(i);
			}
		}	

		for (int32 IndexToRemove : RemovedEventIndices)
		{		
			STEvents[Tag].RemoveEvent(IndexToRemove);
		}
	}
}
