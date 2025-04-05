// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "Components/SplineComponent.h"
#include "ANS_CameraSpline.generated.h"

UENUM(Blueprintable,BlueprintType)
enum class ESplinePackEditMode : uint8
{
	None,
	Save,
	Load,
	Clear
};

UCLASS(Blueprintable,BlueprintType)
class ASplinePackGenerator : public AActor
{
	GENERATED_BODY()
public:
	ASplinePackGenerator();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)override;

	virtual void PostLoad()override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* EditingSplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESplinePackEditMode EditMode;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<UCameraSplinePack> SplinePackToModify;
};

UCLASS(Blueprintable,BlueprintType)
class UCameraSplinePack : public UObject
{
	GENERATED_BODY()
public:	
	// 스플라인 해석함수
	FTransform GetTransformAtSplineInputKey(AActor* Owner,const FVector& CameraLocation, float InKey, ESplineCoordinateSpace::Type CoordinateSpace, bool bUseScale = false) const;

private:
	FVector GetLocationAtSplineInputKey(const FTransform& CameraTransform, float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const;
	FQuat GetQuaternionAtSplineInputKey(const FTransform& CameraTransform, float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const;
	FVector GetScaleAtSplineInputKey(float InKey) const;


public:
	// 스플라인, 요거 스플라인컴포넌트에서 추출해서 사용가능하게하면 재생기능만 있는 스플라인팩가능함
	UPROPERTY()
	FSplineCurves SplineCurves;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAlwaysLookAtOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InterpSpeed = 10.f;
};

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
	UCameraSplinePack* SplinePack;
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
	TSubclassOf<UCameraSplinePack> SplinePack;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowDebug;
#endif
};
