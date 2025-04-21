// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "STPopupWidgetInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTPopupWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

DECLARE_DYNAMIC_DELEGATE(FOnPopUpConfirmEvent);

class STUISYSTEM_API ISTPopupWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnPopupWidgetShow(UUserWidget* InternalWidget, UObject* PopupData);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void BindOnConfirmEvent(const FOnPopUpConfirmEvent& Event);

	UFUNCTION(BlueprintNativeEvent)
	void MarkInUse(bool bInUse);

	virtual bool IsInUse() abstract;
	
};
