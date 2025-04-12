// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "Components/SplineComponent.h"
#include "ANS_CameraSpline.generated.h"


class USTSplinePack;

UCLASS()
class UANS_CameraSplineScratchPad : public UANS_ScratchPad
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AActor* Owner;

	float ElapsedTime = 0.f;

	UPROPERTY()
	class UCameraComponent* CameraComponent;

	UPROPERTY()
	class USpringArmComponent* SpringArmComponent;
	
	UPROPERTY()
	USTSplinePack* SplinePack;
};


UCLASS()
class PROJECTST_API UANS_CameraSpline : public UANS_Base
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer)override;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	TSubclassOf<USTSplinePack> SplinePack;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bEndBlend;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,meta = (EditCondition = "bEndBlend == true "))
	float EndBlendRatio = 0.2f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowDebug;
#endif
};
