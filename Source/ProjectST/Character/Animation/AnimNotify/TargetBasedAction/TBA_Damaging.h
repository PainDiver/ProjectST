// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../ANS_Target.h"
#include "Character/Game/Params.h"
#include "TBA_Damaging.generated.h"

/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API UTBA_DamagingBase : public UTargetBasedAction
{
public:
	GENERATED_BODY()

private:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,Instanced,meta=(AllowPrivateAccess = "true"))
	TArray<UHitParam*> HitParams;
};
