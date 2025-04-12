// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "STSplinePack.generated.h"

UENUM(Blueprintable,BlueprintType)
enum class ESplinePackEditMode : uint8
{
	None,
	Save,
	Load,
	Clear
};

UCLASS(Blueprintable,BlueprintType)
class ASTSplinePackGenerator : public AActor
{
	GENERATED_BODY()
public:
	ASTSplinePackGenerator();

	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)override;

	virtual void PostLoad()override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USplineComponent* EditingSplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ESplinePackEditMode EditMode;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TSubclassOf<USTSplinePack> SplinePackToModify;
};

UCLASS(Blueprintable,BlueprintType)
class USTSplinePack : public UObject
{
	GENERATED_BODY()
public:	
	// 스플라인 해석함수
	FTransform GetTransformAtSplineInputKey(const FTransform& OwnerTransform, const FVector& FocalStart,const FVector& FocalPoint, float InKey, ESplineCoordinateSpace::Type CoordinateSpace,bool bUseOnlyYaw = true ,bool bUseScale = false) const;

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
