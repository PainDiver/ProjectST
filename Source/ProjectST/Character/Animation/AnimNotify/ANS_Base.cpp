// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "../STAnimInstance.h"

UWorld* UANS_Condition::GetWorld() const
{

#if WITH_EDITOR
	return GEngine->GetCurrentPlayWorld();
#else
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
#endif
}


UANS_Base::UANS_Base()
{	
	if(ScratchPadKey ==0)
		ScratchPadKey = GetUniqueID();
}

bool UANS_Base::CheckCondition(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{	
	if (MeshComp == nullptr)
	{
		return false;
	}

	if (!CheckRealmCondition(MeshComp,EventReference))
	{
		return false;
	}


	if (UANS_ScratchPad* ScratchPad = GetCachedScratchPad(MeshComp->GetAnimInstance()))
	{
		if (ScratchPad->RemainingCount == 0)
		{
			return false;
		}
	}

	bool bMetConditon = Conditions.Num() == 0;
	for (UANS_Condition* Condition : Conditions)
	{		
		if (Condition == nullptr)
		{
			continue;
		}

		if (!Condition->CanProcess(MeshComp, Animation, EventReference, GetInterestedScratchPad(MeshComp->GetAnimInstance())))
		{
			if(bShouldMeetAllCondition)
				return false;
		}
		else if (!bShouldMeetAllCondition)
		{
			bMetConditon = true;
			break;
		}
	}

	if (!bShouldMeetAllCondition && !bMetConditon)
		return false;

	return true;
}

bool UANS_Base::CheckRealmCondition(USkeletalMeshComponent* MeshComp, const FAnimNotifyEventReference& EventReference)
{
	// ShouldTrigger 저거는 Queue에서만 호출되므로 Branching도 호출시켜야함
	if (EventReference.GetNotify() == nullptr || EventReference.GetNotify()->MontageTickType == EMontageNotifyTickType::BranchingPoint)
	{
		if (USTAnimInstance* AnimInstance = Cast<USTAnimInstance>(MeshComp->GetAnimInstance()))
		{
			if (!AnimInstance->ShouldTriggerAnimNotifyState(this))
			{
				return false;
			}
		}
	}
	return true;
}

void UANS_Base::DecreaseChanceCount(UAnimInstance* AnimInstance)
{
	if (UANS_ScratchPad* ScratchPad = GetCachedScratchPad(AnimInstance))
	{
		ScratchPad->RemainingCount -= 1;
	}
}

void UANS_Base::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UAnimNotifyState::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (USTAnimInstance* AnimInstance = Cast<USTAnimInstance>(MeshComp->GetAnimInstance()))
	{
		if (UANS_ScratchPad* ScratchPad = CreateScratchPad(AnimInstance))
		{
			ScratchPad->Animation = Animation;
			AnimInstance->CacheScratchPad(ScratchPadKey, ScratchPad);
			ScratchPad->RemainingCount = LimitedCount;
		}
	}
}

void UANS_Base::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UAnimNotifyState::NotifyEnd(MeshComp, Animation, EventReference);

	if (UANS_ScratchPad* ScratchPad = GetCachedScratchPad(MeshComp->GetAnimInstance()))
	{
		ScratchPad->bMarkDead = true;
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

UANS_ScratchPad* UANS_Base::GetCachedScratchPad(UAnimInstance* AnimInstance)
{	
	if (USTAnimInstance* STAnimInstance = Cast<USTAnimInstance>(AnimInstance))
	{		
		return STAnimInstance->GetCachedScratchPad(ScratchPadKey);
	}
	return nullptr;
}

UANS_ScratchPad* UANS_Base::GetInterestedScratchPad(UAnimInstance* AnimInstance)
{
	if (USTAnimInstance* STAnimInstance = Cast<USTAnimInstance>(AnimInstance))
	{
		return STAnimInstance->GetCachedScratchPad(ANSUniqueKeyForScratchPad);
	}
	return nullptr;
}

void UANS_Base::PostLoad()
{
	Super::PostLoad();

	if (ScratchPadKey == 0)
		ScratchPadKey = GetUniqueID();
}

void UANS_Base::PostDuplicate(bool bDuplicateForPIE)
{
	ScratchPadKey = GetUniqueID();
}
