// Fill out your copyright notice in the Description page of Project Settings.


#include "STCharacterMovementComponent.h"
#include "Character/STStateInterface.h"
#include "Game/STNativeGameplayTag.h"

USTCharacterMovementComponent::USTCharacterMovementComponent()
	:Super()
{
	RotationRate = FRotator(0.f, DefaultStat.RotationRate, 0.f);
	MaxWalkSpeed = DefaultStat.NormalSpeed;
}

void USTCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USTCharacterMovementComponent::ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)
{
	Super::ProcessLanded(Hit,remainingTime,Iterations);
	
	ISTStateInterface::Execute_AddState(PawnOwner, State_Standing);

}

bool USTCharacterMovementComponent::DoJump(bool bReplayingMoves, float DeltaTime)
{
	if (Super::DoJump(bReplayingMoves,DeltaTime))
	{		
		ISTStateInterface::Execute_AddState(PawnOwner, State_Falling);
		return true;
	}
	return false;
}
