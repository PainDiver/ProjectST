// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STPopupInternalWidgetInterface.h"
#include "STPopupInternalUserWidget.generated.h"

/**
 * 
 */
class USTPopupUserWidget;
UCLASS()
class STUISYSTEM_API USTPopupInternalUserWidget : public UUserWidget, public ISTPopupInternalWidgetInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	USTPopupUserWidget* GetParentUserWidget();
};
