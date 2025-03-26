// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STManagedStates.h"
#include "Character/STCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Game/STNativeGameplayTag.h"

void USTManagedState::OnTick_Implementation(AActor* StateOwner,float DeltaTime)
{	
	if (bIsRemoved)
	{
		bIsRemoved = false;
		OnStateAdded(StateOwner);
	}
}

bool USTManagedState::IsMatchingState(const FGameplayTag Tag)
{
	return StateTag == StateTag;
}

USTManagedState_Guard::USTManagedState_Guard()
{
	TagNotAllowed.AddTag(State_Falling);
	TagNotAllowed.AddTag(State_Lying);
	TagNotAllowed.AddTag(State_Dead);
}

void USTManagedState_Guard::OnStateAdded_Implementation(AActor* StateOwner)
{
	if (MovementComp == nullptr)
	{
		if (ACharacter* Character = Cast<ACharacter>(StateOwner))
		{
			MovementComp = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
		}
	}

	if (MovementComp == nullptr)
		return;

	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true;
	MovementComp->MaxWalkSpeed = MovementComp->GetDefaultMovementStat().GuardingSpeed;
}

void USTManagedState_Guard::OnTick_Implementation(AActor* StateOwner, float DeltaTime)
{
	USTManagedState::OnTick_Implementation(StateOwner,DeltaTime);

	FGameplayTagContainer Container = ISTStateInterface::Execute_GetStates(StateOwner);
	if (Container.HasAny(TagNotAllowed))
	{
		if(StateOwner->HasAuthority())
		{ 
			ISTStateInterface::Execute_RemoveState_Replication(StateOwner,CombatState_Guard);
		}
		OnStateRemoved(StateOwner);
	}
}

void USTManagedState_Guard::OnStateRemoved_Implementation(AActor* StateOwner)
{
	USTManagedState::OnStateRemoved_Implementation(StateOwner);

	if (MovementComp == nullptr)
	{
		if (ACharacter* Character = Cast<ACharacter>(StateOwner))
		{
			MovementComp = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
		}
	}

	if (MovementComp == nullptr)
		return;

	MovementComp->bOrientRotationToMovement = true;
	MovementComp->bUseControllerDesiredRotation = false;
	MovementComp->MaxWalkSpeed = MovementComp->GetDefaultMovementStat().NormalSpeed;
}
