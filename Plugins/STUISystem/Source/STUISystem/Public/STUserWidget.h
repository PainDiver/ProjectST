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
	
	void OnWidgetRegistered_Implementation() {};
	
	void OnWidgetOnTop_Implementation();

	virtual bool IsPersistentWidget()override { return bIsPersistent; };

	virtual EWidgetModalType GetWidgetModalType()override{ return ModalType; }

	virtual void SetWidgetController(TObjectPtr<USTWidgetController> NewController) override;

private:

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTWidgetController> WidgetController;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta=(AllowPrivateAccess="true"))
	EWidgetModalType ModalType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	EWidgetInputMode InputMode;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bIsPersistent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	bool bShowMouseCursor;
};
