// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_SendEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UANS_SendEvent::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);

	SendEvent(MeshComp);
	DecreaseChanceCount(MeshComp->GetAnimInstance());
}

void UANS_SendEvent::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);

	SendEvent(MeshComp);
	DecreaseChanceCount(MeshComp->GetAnimInstance());
}

void UANS_SendEvent::SendEvent(USkeletalMeshComponent* MeshComp)
{
	FGameplayEventData Data;
	Data.Instigator = MeshComp->GetOwner();
	Data.Target = MeshComp->GetOwner();
	Data.OptionalObject = ObjectToSend;
	Data.EventTag = EventTag;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag,Data);

	DecreaseChanceCount(MeshComp->GetAnimInstance());
}
