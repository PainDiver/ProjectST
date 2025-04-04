// Fill out your copyright notice in the Description page of Project Settings.


#include "STManagedStates.h"
#include "Net/UnrealNetwork.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/STStateHandlingComponent.h"

void USTManagedState::OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	bIsRemoved = false;
}

void USTManagedState::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	bIsRemoved = true;	
}

bool USTManagedState::IsMatchingState(const FGameplayTag Tag)
{
	return StateTag == StateTag;
}

void USTManagedState::ResolveCollapsingStates(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	FGameplayTagContainer Container = ISTStateInterface::Execute_GetStates(StateOwner);
	for (const FGameplayTag& Tag : TagNotAllowed)
	{
		if (Container.HasTag(Tag))
		{
			ISTStateInterface::Execute_RemoveState_Replication(StateOwner, GetTag());
			OnStateRemoved(StateOwner, OwnerComponent);
			OwnerComponent->RestoreLastStateEffect();
		}
	}
}

