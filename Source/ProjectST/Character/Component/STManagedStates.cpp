// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STManagedStates.h"
#include "Character/STCharacterBase.h"
#include "Net/UnrealNetwork.h"
#include "Character/Component/STCharacterMovementComponent.h"


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

void USTManagedState_Guard::OnStateAdded_Implementation(AActor* StateOwner)
{
	ASTCharacterBase* Character = Cast<ASTCharacterBase>(StateOwner);
	if (Character == nullptr)
	{
		return;
	}
	USTCharacterMovementComponent* MovementComp = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
	if (MovementComp == nullptr)
	{
		return;
	}

	MovementComp->bOrientRotationToMovement = false;
	MovementComp->bUseControllerDesiredRotation = true;
	MovementComp->MaxWalkSpeed = 175.f;
}

void USTManagedState_Guard::OnTick_Implementation(AActor* StateOwner, float DeltaTime)
{
	USTManagedState::OnTick_Implementation(StateOwner,DeltaTime);
}

void USTManagedState_Guard::OnStateRemoved_Implementation(AActor* StateOwner)
{
	USTManagedState::OnStateRemoved_Implementation(StateOwner);
	ASTCharacterBase* Character = Cast<ASTCharacterBase>(StateOwner);
	if (Character == nullptr)
	{
		return;
	}
	USTCharacterMovementComponent* MovementComp = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
	if (MovementComp == nullptr)
	{
		return;
	}

	MovementComp->bOrientRotationToMovement = true;
	MovementComp->bUseControllerDesiredRotation = false;
	MovementComp->MaxWalkSpeed = MovementComp->GetDefaultMovementStat().MaxWalkSpeed;
}
