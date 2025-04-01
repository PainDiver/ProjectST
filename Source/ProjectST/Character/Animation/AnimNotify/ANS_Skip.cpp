// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Skip.h"

void UANS_Skip::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration,EventReference);
	if (!CheckCondition(MeshComp,Animation,EventReference))
	{
		return;
	}

}

void UANS_Skip::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!CheckCondition(MeshComp, Animation, EventReference))
	{
		return;
	}

	MeshComp->GetAnimInstance()->Montage_SetPosition(EventReference.GetNotify()->GetLinkedMontage(), TargetPosition);

	DecreaseChanceCount(MeshComp->GetAnimInstance());
}

void UANS_Skip::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

}
