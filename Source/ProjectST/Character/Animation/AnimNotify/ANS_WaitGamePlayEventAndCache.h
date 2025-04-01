// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "AbilitySystemComponent.h"
#include "ANS_WaitGamePlayEventAndCache.generated.h"

UCLASS()
class UANS_WaitGameplayEventAndCacheScratchPad : public UANS_ScratchPad
{
	GENERATED_BODY()
public:

	FDelegateHandle Handle;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	FGameplayEventData Payload;
};

UCLASS()
class PROJECTST_API UANS_WaitGamePlayEventAndCache : public UANS_Base
{
	GENERATED_BODY()

public:

	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer)override;

	void GameplayEventCallback(const FGameplayEventData* Payload);


	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FGameplayTag EventTag;

};
