// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Skip.h"

UANS_ScratchPad* UANS_Skip::CreateScratchPad(UObject* Outer)
{
	return NewObject<UANS_ScratchPad_Skip>(Outer);
}

void UANS_Skip::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration,EventReference);
	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);



}

void UANS_Skip::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);

	if (UANS_ScratchPad_Skip* ScratchPad = Cast<UANS_ScratchPad_Skip>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (EventReference.GetNotify())
		{
			ScratchPad->bSkipSucceded = true;
			MeshComp->GetAnimInstance()->Montage_SetPosition(EventReference.GetNotify()->GetLinkedMontage(), EventReference.GetNotify()->GetEndTriggerTime());
			DecreaseChanceCount(MeshComp->GetAnimInstance());
		}
	}
}

void UANS_Skip::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UANS_ScratchPad_Skip* ScratchPad = Cast<UANS_ScratchPad_Skip>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (bShouldLoopOnFail && !ScratchPad->bSkipSucceded)
		{
			MeshComp->GetAnimInstance()->Montage_SetPosition(EventReference.GetNotify()->GetLinkedMontage(), EventReference.GetNotify()->GetTriggerTime());
		}
	}
}
