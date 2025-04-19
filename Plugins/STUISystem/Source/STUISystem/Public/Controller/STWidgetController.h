// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STWidgetController.generated.h"


UENUM(BlueprintType)
enum class EWidgetControllerType: uint8
{
	HUDBaseController,
	StatusBaseController
};

UCLASS(Blueprintable,BlueprintType)
class STUISYSTEM_API USTWidgetController: public UObject
{
	GENERATED_BODY()
public:
	virtual void OnWidgetControllerSet(APlayerController* Owner) {};

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EWidgetControllerType ControllerType;
};

