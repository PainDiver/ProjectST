// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Turn.h"
#include "ANS_Turn.h"
#include "GameFramework/Character.h"
#include "Character/Component/STCharacterMovementComponent.h"

void UANS_Turn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{	
	ACharacter* Character = GetOwningCharacter<ACharacter>(MeshComp);
	if (Character == nullptr)
	{
		return;
	}
	UCharacterMovementComponent* CM = Character->GetCharacterMovement();
	if (CM == nullptr)
	{
		return;
	}
	 
	CM->RotationRate = FRotator(0.f,RotationSpeedPerSecond,0.f);
	CM->bOrientRotationToMovement = false;
	CM->bUseControllerDesiredRotation = true;
	CM->bAllowPhysicsRotationDuringAnimRootMotion = true;
}

void UANS_Turn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	ACharacter* Character = GetOwningCharacter<ACharacter>(MeshComp);
	if (Character == nullptr)
	{
		return;
	}
	USTCharacterMovementComponent* CM = Cast<USTCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CM == nullptr)
	{
		return;
	}	

	CM->RotationRate = FRotator(0.f,CM->GetDefaultMovementStat().RotationRate,0.f);
	CM->bOrientRotationToMovement = true;
	CM->bUseControllerDesiredRotation = false;
	CM->bAllowPhysicsRotationDuringAnimRootMotion = false;
}
