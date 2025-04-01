// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_WaitGamePlayEventAndCache.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Character/Animation/STAnimInstance.h"
#include "AbilitySystemGlobals.h"

UANS_ScratchPad* UANS_WaitGamePlayEventAndCache::CreateScratchPad(UObject* Outer)
{
	return NewObject<UANS_WaitGameplayEventAndCacheScratchPad>(Outer);
}

void UANS_WaitGamePlayEventAndCache::GameplayEventCallback(const FGameplayEventData* Payload)
{
	if (const ACharacter* Character = Cast<ACharacter>(Payload->Instigator))
	{
		if (USTAnimInstance* Instance = Cast<USTAnimInstance>(Character->GetMesh()->GetAnimInstance()))
		{
			if (UANS_WaitGameplayEventAndCacheScratchPad* ScratchPad = Cast<UANS_WaitGameplayEventAndCacheScratchPad>(Instance->GetCachedScratchPad(ScratchPadKey)))
			{			
				ScratchPad->Payload = *Payload;
			}
		}
	}
}

void UANS_WaitGamePlayEventAndCache::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UANS_WaitGameplayEventAndCacheScratchPad* ScratchPad = Cast<UANS_WaitGameplayEventAndCacheScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
		{
			ScratchPad->Handle = ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).AddUObject(this, &UANS_WaitGamePlayEventAndCache::GameplayEventCallback);
		}
	}
}

void UANS_WaitGamePlayEventAndCache::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UANS_WaitGameplayEventAndCacheScratchPad* ScratchPad = Cast<UANS_WaitGameplayEventAndCacheScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner()))
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(EventTag).Remove(ScratchPad->Handle);
		}
	}
}
