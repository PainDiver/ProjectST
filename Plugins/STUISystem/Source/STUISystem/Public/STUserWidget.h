// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "STUIEnum.h"
#include "STWidgetInterface.h"
#include "STUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class STUISYSTEM_API USTUserWidget : public UUserWidget, 
	public ISTWidgetInterface
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetShow();

	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetHide();

	void OnWidgetOnTop_Implementation();

	virtual bool IsPersistentWidget()override { return bIsPersistent; };

	virtual EWidgetModalType GetWidgetModalType()override{ return ModalType; }

private:

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta=(AllowPrivateAccess="true"))
	EWidgetModalType ModalType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EWidgetInputMode InputMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bIsPersistent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bShowMouseCursor;
};
