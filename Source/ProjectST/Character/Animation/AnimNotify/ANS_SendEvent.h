// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "GameplayTagcontainer.h"
#include "ANS_SendEvent.generated.h"

UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class UANS_EventObject :public UObject
{
	GENERATED_BODY()
};

UCLASS()
class PROJECTST_API UANS_SendEvent : public UANS_Base
{
	GENERATED_BODY()
	
public:	
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;

	void SendEvent(USkeletalMeshComponent* MeshComp);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag EventTag;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Instanced)
	UANS_EventObject* ObjectToSend;
};
