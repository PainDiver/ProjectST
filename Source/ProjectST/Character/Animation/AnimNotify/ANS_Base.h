// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../STEnum_Anim.h"
#include "ANS_Base.generated.h"

UCLASS(Abstract)
class UANS_ScratchPad : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class PROJECTST_API UANS_Base : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

	virtual FString GetUniqueKey(const FAnimNotifyEvent* NotifyEvent) { return FString(); };
	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer) { return nullptr; }
	
	UShapeComponent* GetCachedShape(UAnimInstance* AnimInstance, ETargetQueryType Type);

	UANS_ScratchPad* GetCachedScratchPad(UAnimInstance* AnimInstance, const FString& Key);

	template<typename T>
	T* GetOwningCharacter(USkeletalMeshComponent* MeshComp)
	{
		if (MeshComp)
		{
			return Cast<T>(MeshComp->GetOwner());
		}
		return nullptr;
	}

	
};
