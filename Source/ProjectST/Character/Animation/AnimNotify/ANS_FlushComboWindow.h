// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_Base.h"
#include "ANS_FlushComboWindow.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API UANS_FlushComboWindow : public UANS_Base
{
	GENERATED_BODY()
	
public:
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;
	
	
};
