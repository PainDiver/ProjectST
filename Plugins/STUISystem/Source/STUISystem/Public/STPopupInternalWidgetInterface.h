// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "STPopupInternalWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTPopupInternalWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class STUISYSTEM_API ISTPopupInternalWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnPopupInternalWidgetShow(UUserWidget* BaseWidget,UObject* PopUpData);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void OnPopupInternalWidgetClose();

};
