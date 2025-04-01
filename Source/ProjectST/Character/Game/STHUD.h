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
	
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable)
	bool RegisterWidget(FGameplayTag Tag, UUserWidget* Widget);

	UFUNCTION(BlueprintCallable)
	bool ShowWidget(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetWidgetByTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable)
	bool CloseLatestWidget();

	UFUNCTION()
	void CloseWidget(const FInputActionInstance& Instance);


private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UInputAction* CloseWidgetInput;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess= "true"))
	TSubclassOf<UUserWidget> BlockingWidgetClass;

	UPROPERTY()
	UUserWidget* BlockingWidget;

	UPROPERTY()
	TArray<FGameplayTag> WidgetStack;

	UPROPERTY()
	TMap<FGameplayTag, UUserWidget*> WidgetBases;
};
