// Fill out your copyright notice in the Description page of Project Settings.


#include "STManagedState_Sprint.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/STStateHandlingComponent.h"


USTManagedState_Sprint::USTManagedState_Sprint()
{
	TagNotAllowed.AddTag(State_Lying);
	TagNotAllowed.AddTag(State_Dead);
	TagNotAllowed.AddTag(CombatState_Hit);
	TagNotAllowed.AddTag(CombatState_Guard);
	TagNotAllowed.AddTag(CombatState_Attack);
	TagNotAllowed.AddTag(CombatState_Evade);

	TagNotAllowed.AddTag(Parkour_Climb);
	TagNotAllowed.AddTag(Parkour_Vault);
	TagNotAllowed.AddTag(Parkour_Mantle);
	

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

	return MovementComp->Velocity.Size() > 10.f;
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
	USTManagedState::OnTick_Implementation(StateOwner, OwnerComponent, DeltaTime);

}

void USTManagedState_Sprint::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
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
