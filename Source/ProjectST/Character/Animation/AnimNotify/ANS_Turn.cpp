// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Turn.h"
#include "ANS_Turn.h"
#include "GameFramework/Character.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Character/Component/STMotionWarpingComponent.h"


void UANS_Turn::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{		
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!CheckCondition(MeshComp, Animation, EventReference))
		return;


	if (UANS_ScratchPad_Turn* ScratchPad = Cast<UANS_ScratchPad_Turn>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
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
	 
		CM->RotationRate = FRotator(0.f,RotationSpeedPerSecond,0.f);
		CM->bOrientRotationToMovement = false;
		CM->bUseControllerDesiredRotation = true;
		CM->bAllowPhysicsRotationDuringAnimRootMotion = true;

		ScratchPad->MotionWarpingComponent = Character->GetComponentByClass<USTMotionWarpingComponent>();
		ScratchPad->MovementComponent = CM;
	}
}

void UANS_Turn::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (UANS_ScratchPad_Turn* ScratchPad = Cast<UANS_ScratchPad_Turn>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (ScratchPad->MotionWarpingComponent && ScratchPad->MotionWarpingComponent->IsWarping())
		{
			SetBackToDefault(ScratchPad->MovementComponent);
		}		
	}
}

void UANS_Turn::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UANS_ScratchPad_Turn* ScratchPad = Cast<UANS_ScratchPad_Turn>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		SetBackToDefault(ScratchPad->MovementComponent);
	}
}

void UANS_Turn::SetBackToDefault(USTCharacterMovementComponent* MovementComponent)
{
	if (MovementComponent)
	{
		MovementComponent->RotationRate = FRotator(0.f, MovementComponent->GetDefaultMovementStat().RotationRate, 0.f);
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = false;
	}

}
