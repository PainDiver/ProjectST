// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STStateHandlingComponent.h"
#include "STManagedStates.h"
#include "Engine/ActorChannel.h"
#include "Character/STStateInterface.h"

// Sets default values for this component's properties
USTStateHandlingComponent::USTStateHandlingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTStateHandlingComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();
}

// Called every frame
void USTStateHandlingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FGameplayTagContainer StatesContainer = ISTStateInterface::Execute_GetStates(Owner);	
	for (USTManagedState* State : States)
	{
		if (State == nullptr)
			continue;

		if (StatesContainer.HasTag(State->GetTag()))
		{
			State->OnTick(Owner,this,DeltaTime);
		}
		else if(!State->IsRemoved())
		{
			State->OnStateRemoved(Owner,this);
		}
	}

}

void USTStateHandlingComponent::RestoreLastStateEffect()
{
	if (StateOnRunning.Num() > 0)
	{
		const FGameplayTag& Tag = StateOnRunning.Last();
		USTManagedState** MatchedState = States.FindByPredicate([Tag](USTManagedState* State)
			{
				return State->GetTag() == Tag;
			});

		if (MatchedState)
		{
			(*MatchedState)->OnStateAdded(GetOwner(),this);
		}
	}
}

void USTStateHandlingComponent::AddToStateOnRunning(const FGameplayTag& NewStack)
{
	StateOnRunning.AddUnique(NewStack);
}

void USTStateHandlingComponent::RemoveStateOnRunning(const FGameplayTag& NewStack)
{
	StateOnRunning.Remove(NewStack);
}

bool USTStateHandlingComponent::CanAddState(const FGameplayTag& Tag)
{
	USTManagedState** FoundState = States.FindByPredicate([Tag](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == Tag;
			}
			return false;
		});

	if (FoundState)
	{
		return (*FoundState)->CanAddState(GetOwner());
	}

	return true;
}
