// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STManagedStates.h"
#include "Character/STCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "STStateHandlingComponent.h"

void USTManagedState::OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	OwnerComponent->AddToStateOnRunning(GetTag());
	bIsRemoved = false;
}

void USTManagedState::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	OwnerComponent->RemoveStateOnRunning(GetTag());
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
			if (StateOwner->HasAuthority())
			{
				ISTStateInterface::Execute_RemoveState_Replication(StateOwner, GetTag());
			}
			OnStateRemoved(StateOwner, OwnerComponent);
			OwnerComponent->RestoreLastStateEffect();
		}
	}
}

USTManagedState_Guard::USTManagedState_Guard()
{
	TagNotAllowed.AddTag(State_Falling);
	TagNotAllowed.AddTag(State_Lying);
	TagNotAllowed.AddTag(State_Dead);
	TagNotAllowed.AddTag(CombatState_Hit);
	TagNotAllowed.AddTag(CombatState_Sprint);
	TagNotAllowed.AddTag(CombatState_Attack);
	TagNotAllowed.AddTag(CombatState_Evade);
}

void USTManagedState_Guard::OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateAdded_Implementation(StateOwner, OwnerComponent);

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

void USTManagedState_Guard::OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)
{
	USTManagedState::OnTick_Implementation(StateOwner, OwnerComponent,DeltaTime);
}

void USTManagedState_Guard::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateRemoved_Implementation(StateOwner, OwnerComponent);

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

USTManagedState_Sprint::USTManagedState_Sprint()
{
	TagNotAllowed.AddTag(State_Falling);
	TagNotAllowed.AddTag(State_Lying);
	TagNotAllowed.AddTag(State_Dead);
	TagNotAllowed.AddTag(CombatState_Hit);
	TagNotAllowed.AddTag(CombatState_Guard);
	TagNotAllowed.AddTag(CombatState_Attack);
	TagNotAllowed.AddTag(CombatState_Evade);
}

bool USTManagedState_Sprint::CanAddState_Implementation(AActor* StateOwner)
{
	if (MovementComp == nullptr)
	{
		if (ACharacter* Character = Cast<ACharacter>(StateOwner))
		{
			MovementComp = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
		}
	}

	if (MovementComp == nullptr)
		return false;

	return MovementComp->Velocity.Size() > 50.f;
}

void USTManagedState_Sprint::OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateAdded_Implementation(StateOwner, OwnerComponent);
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
	MovementComp->MaxWalkSpeed = MovementComp->GetDefaultMovementStat().SprintSpeed;
}

void USTManagedState_Sprint::OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)
{
	USTManagedState::OnTick_Implementation(StateOwner, OwnerComponent,DeltaTime);

}

void USTManagedState_Sprint::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateRemoved_Implementation(StateOwner,OwnerComponent);
	OwnerComponent->RemoveStateOnRunning(GetTag());

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
