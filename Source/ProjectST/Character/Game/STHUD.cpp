// Fill out your copyright notice in the Description page of Project Settings.


#include "STHUD.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"



void ASTHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetOwningPlayerController() && BlockingWidgetClass)
	{
		BlockingWidget = CreateWidget(GetOwningPlayerController(), BlockingWidgetClass, TEXT("BGBlockingWidget"));
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(GetOwningPlayerController()->InputComponent))
		{
			EnhancedInputComponent->BindAction(CloseWidgetInput, ETriggerEvent::Started, this, &ASTHUD::CloseWidget);
		}
	}
}

bool ASTHUD::RegisterWidget(FGameplayTag Tag, UUserWidget* Widget)
{
	if (ISTWidgetInterface* WidgetInterface = Cast<ISTWidgetInterface>(Widget))
	{
		WidgetBases.Add(Tag, Widget);
		ISTWidgetInterface::Execute_OnWidgetRegistered(Widget);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Must be ISTWidgetInterface to Register"));
	return false;
}

bool ASTHUD::ShowWidget(FGameplayTag Tag)
{
	if (!WidgetBases.Contains(Tag))
	{
		return false;
	}
	
	UUserWidget* Widget = WidgetBases[Tag];
	ISTWidgetInterface* WidgetInterface = Cast<ISTWidgetInterface>(Widget);
	
	if (WidgetInterface == nullptr)
		return false;
	
	ISTWidgetInterface::Execute_OnWidgetShow(Widget);
	if (!WidgetInterface->IsPersistentWidget())
	{
		WidgetStack.AddUnique(Tag);
		ISTWidgetInterface::Execute_OnWidgetOnTop(Widget);
		if (WidgetInterface->GetWidgetModalType() == EWidgetModalType::Modal)
		{
			//Blocking Background 추가
			BlockingWidget->AddToViewport();
		}
	}
	Widget->AddToViewport();
	
	return true;
}

UUserWidget* ASTHUD::GetWidgetByTag(FGameplayTag Tag)
{
	if (WidgetBases.Contains(Tag))
	{
		return WidgetBases[Tag];
	}

	return nullptr;
}

bool ASTHUD::CloseLatestWidget()
{
	if (WidgetStack.Num() <= 0)
		return false;

	FGameplayTag LastWidgetTag = WidgetStack.Last();
	UUserWidget* Widget = WidgetBases[LastWidgetTag];
	ISTWidgetInterface::Execute_OnWidgetRemoved(Widget);
	Widget->RemoveFromParent();
	WidgetStack.RemoveAt(WidgetStack.Num() - 1);

	if (WidgetStack.Num() > 0)
	{
		LastWidgetTag = WidgetStack.Last();
		Widget = WidgetBases[LastWidgetTag];
		ISTWidgetInterface* WidgetInterface = Cast<ISTWidgetInterface>(Widget);
		if (WidgetInterface->GetWidgetModalType() == EWidgetModalType::Modal)
		{
			//Blocking Background 추가
			BlockingWidget->AddToViewport();
			Widget->AddToViewport();
		}
		ISTWidgetInterface::Execute_OnWidgetShow(Widget);
		ISTWidgetInterface::Execute_OnWidgetOnTop(Widget);
	}
	else
	{
		BlockingWidget->RemoveFromParent();
	}

	return true;
}

void ASTHUD::CloseWidget(const FInputActionInstance& Instance)
{
	CloseLatestWidget();
}
