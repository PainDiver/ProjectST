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
		if (StatesContainer.HasTag(State->GetTag()))
		{
			State->OnTick(Owner, DeltaTime);
		}
		else if(!State->IsRemoved())
		{
			State->OnStateRemoved(Owner);
		}
	}

}
