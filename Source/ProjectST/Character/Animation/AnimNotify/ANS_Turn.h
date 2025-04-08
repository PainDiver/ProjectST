// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "ANS_Turn.generated.h"

class USTCharacterMovementComponent;
class USTMotionWarpingComponent;

UCLASS()
class UANS_ScratchPad_Turn :public UANS_ScratchPad
{
	GENERATED_BODY()

public:
	UPROPERTY()
	USTCharacterMovementComponent* MovementComponent;

	UPROPERTY()
	USTMotionWarpingComponent* MotionWarpingComponent;
};

UCLASS()
class PROJECTST_API UANS_Turn : public UANS_Base
{
	GENERATED_BODY()
	
public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;

	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer);

	void SetBackToDefault(USTCharacterMovementComponent* MovementComponent);


	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float RotationSpeedPerSecond = 360;
};
