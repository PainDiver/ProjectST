// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controller/STWidgetController.h"
#include "STWidgetController_Status.generated.h"

/**
 * 
 */
class USTInventoryComponent;

UCLASS()
class PROJECTST_API USTWidgetController_Status : public USTWidgetController
{
	GENERATED_BODY()
	
public:
	USTWidgetController_Status();

	virtual void OnWidgetControllerSet(APlayerController* Owner)override;
	
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	USTInventoryComponent* InventoryComp;

};
