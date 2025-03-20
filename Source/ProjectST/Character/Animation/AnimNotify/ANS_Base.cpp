// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "../STAnimInstance.h"

void UANS_Base::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UAnimNotifyState::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (USTAnimInstance* AnimInstance = Cast<USTAnimInstance>(MeshComp->GetAnimInstance()))
	{
		if (UANS_ScratchPad* ScratchPad = CreateScratchPad(AnimInstance))
		{
			AnimInstance->CacheScratchPad(GetUniqueKey(EventReference.GetNotify()), ScratchPad);
		}
	}
}

void UANS_Base::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UAnimNotifyState::NotifyEnd(MeshComp, Animation, EventReference);

	if (USTAnimInstance* AnimInstance = Cast<USTAnimInstance>(MeshComp->GetAnimInstance()))
	{		
		AnimInstance->RemoveScratchPad(GetUniqueKey(EventReference.GetNotify()));
	}
}


UShapeComponent* UANS_Base::GetCachedShape(UAnimInstance* AnimInstance, ETargetQueryType Type)
{
	if (USTAnimInstance* STAnimInstance = Cast<USTAnimInstance>(AnimInstance))
	{
		return STAnimInstance->GetCachedShape(Type);
	}
	return nullptr;
}

UANS_ScratchPad* UANS_Base::GetCachedScratchPad(UAnimInstance* AnimInstance, const FString& Key)
{	
	if (USTAnimInstance* STAnimInstance = Cast<USTAnimInstance>(AnimInstance))
	{
		return STAnimInstance->GetCachedScratchPad(Key);
	}
	return nullptr;
}

