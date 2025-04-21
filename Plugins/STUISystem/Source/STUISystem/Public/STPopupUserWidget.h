// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STPopupWidgetInterface.h"
#include "STPopupUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUISYSTEM_API USTPopupUserWidget : public UUserWidget, public ISTPopupWidgetInterface
{
	GENERATED_BODY()
	

public:
	virtual bool IsInUse() { return bInUse; };

	void MarkInUse_Implementation(bool bIsInUse) { bInUse = bIsInUse; };



private:
	bool bInUse;
};
