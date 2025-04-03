// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STWidgetController.generated.h"
/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class STUISYSTEM_API USTWidgetController: public UObject
{
	GENERATED_BODY()
public:
	virtual void OnWidgetControllerSet(APlayerController* Owner) {};
};

