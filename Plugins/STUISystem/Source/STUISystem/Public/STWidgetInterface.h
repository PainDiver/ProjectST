// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "STUIEnum.h"
#include "STWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STUISYSTEM_API ISTWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetRegistered();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetShow();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetRemoved();

	UFUNCTION(BlueprintNativeEvent)
	void OnWidgetOnTop();

	UFUNCTION()
	virtual bool IsPersistentWidget() abstract;

	UFUNCTION()
	virtual EWidgetModalType GetWidgetModalType() abstract;

};
