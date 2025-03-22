// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionWarpingComponent.h"
#include "STMotionWarpingComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTMotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	bool IsWarping() { return WarpTargets.Num() > 0; }
};
