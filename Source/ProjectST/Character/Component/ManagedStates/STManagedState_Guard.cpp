// Fill out your copyright notice in the Description page of Project Settings.


#include "STManagedState_Guard.h"
#include "Character/Component/STStateHandlingComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Character/STCharacterBase.h"


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
	USTManagedState::OnTick_Implementation(StateOwner, OwnerComponent, DeltaTime);
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

