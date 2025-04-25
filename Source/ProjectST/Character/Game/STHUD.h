// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InputAction.h"
#include "STUISystem/Public/STWidgetInterface.h"
#include "STHUD.generated.h"


UCLASS()
class PROJECTST_API ASTHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	virtual void BeginPlay()override;


	UFUNCTION(BlueprintCallable)
	bool RegisterWidget(FGameplayTag Tag, UUserWidget* Widget, const TArray<USTWidgetController*>& WidgetController);

	UFUNCTION(BlueprintCallable)
	bool ShowWidget(FGameplayTag Tag,UObject* OpenData);

	UFUNCTION(BlueprintCallable)
	bool ShowPopUp(FGameplayTag Tag,TSubclassOf<UUserWidget> InternalWidget, UObject* PopupData);

	UFUNCTION()
	void OnPopUpWidgetDestructed(UUserWidget* Widget);

	
	UFUNCTION(BlueprintCallable)
	bool CloseWidget(FGameplayTag Tag, UObject* CloseData);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetWidgetByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	bool CloseLatestWidget();

	void ShowBlockingWidget(bool On);


private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UInputAction* CloseWidgetInput;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess= "true"))
	TSubclassOf<UUserWidget> BlockingWidgetClass;

	UPROPERTY()
	UUserWidget* BlockingWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PopupBaseClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<UUserWidget*> PopupBaseWidgets;

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> PopupInternals;

	UPROPERTY()
	TArray<FGameplayTag> WidgetStack;

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> WidgetBases;
};
