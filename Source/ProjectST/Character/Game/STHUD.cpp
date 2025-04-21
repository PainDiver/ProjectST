// Fill out your copyright notice in the Description page of Project Settings.


#include "STHUD.h"
#include "Blueprint/UserWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "STPopupWidgetInterface.h"
#include "STPopupInternalWidgetInterface.h"

const int MaxPopUpCount = 5;

void ASTHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (BlockingWidgetClass)
	{
		BlockingWidget = CreateWidget(GetOwningPlayerController(), BlockingWidgetClass, "BlockingWIdget");
	}

	if (PopupBaseClass)
	{
		for (int i = 0; i < MaxPopUpCount; i++)
		{
			UUserWidget* PopUpWidget = CreateWidget(GetOwningPlayerController(), PopupBaseClass);
			PopUpWidget->OnNativeDestruct.AddUObject(this, &ThisClass::OnPopUpWidgetDestructed);
			PopupBaseWidgets.Add(PopUpWidget);
		}
	}
}

bool ASTHUD::RegisterWidget(FGameplayTag Tag, UUserWidget* Widget, const TArray<USTWidgetController*>& WidgetController)
{
	if (ISTWidgetInterface* WidgetInterface = Cast<ISTWidgetInterface>(Widget))
	{
		WidgetBases.Add(Tag, Widget);
		for(USTWidgetController* Controller :WidgetController)
		{
			WidgetInterface->SetWidgetController(Controller->ControllerType, Controller);
			Controller->OnWidgetControllerSet(GetOwningPlayerController());
		}
		ISTWidgetInterface::Execute_OnWidgetRegistered(Widget);
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("Must be ISTWidgetInterface to Register"));
	return false;
}

bool ASTHUD::ShowWidget(FGameplayTag Tag, UObject* OpenData)
{
	if (!WidgetBases.Contains(Tag))
	{
		return false;
	}
	
	UUserWidget* Widget = WidgetBases[Tag];
	ISTWidgetInterface* WidgetInterface = Cast<ISTWidgetInterface>(Widget);
	
	if (WidgetInterface == nullptr)
		return false;
	
	ISTWidgetInterface::Execute_OnWidgetShow(Widget,OpenData);
	if (!WidgetInterface->IsPersistentWidget())
	{
		WidgetStack.AddUnique(Tag);
		ISTWidgetInterface::Execute_OnWidgetOnTop(Widget);
		if (WidgetInterface->GetWidgetModalType() == EWidgetModalType::Modal)
		{
			//Blocking Background 추가
			ShowBlockingWidget(true);

		}
	}
	Widget->AddToViewport();
	
	return true;
}

bool ASTHUD::ShowPopUp(FGameplayTag Tag, TSubclassOf<UUserWidget> InternalWidget,UObject* PopupData)
{	
	UUserWidget* BaseWidget = nullptr;
	for (UUserWidget* PopUpBase : PopupBaseWidgets)
	{
		ISTPopupWidgetInterface* PopUpWidget = Cast<ISTPopupWidgetInterface>(PopUpBase);
		if (PopUpWidget && !PopUpWidget->IsInUse())
		{
			BaseWidget = PopUpBase;
			ISTPopupWidgetInterface::Execute_MarkInUse(PopUpBase, true);
			break;
		}
	}
		
	if (BaseWidget == nullptr)
	{
		BaseWidget = CreateWidget(GetOwningPlayerController(), PopupBaseClass);
		if (ISTPopupWidgetInterface* PopUpWidget = Cast<ISTPopupWidgetInterface>(BaseWidget))
		{
			ISTPopupWidgetInterface::Execute_MarkInUse(BaseWidget, true);
			BaseWidget->OnNativeDestruct.AddUObject(this, &ThisClass::OnPopUpWidgetDestructed);
			PopupBaseWidgets.Add(BaseWidget);
		}
		else
		{
			return false;
		}
	}

	UUserWidget* PopUpInternal = nullptr;
	if (PopupInternals.Contains(Tag))
	{
		PopUpInternal = PopupInternals[Tag];
	}
	else
	{
		PopUpInternal = CreateWidget(GetOwningPlayerController(), InternalWidget);
		PopupInternals.Add(Tag, PopUpInternal);
	}

	ISTPopupWidgetInterface::Execute_OnPopupWidgetShow(BaseWidget, PopUpInternal, PopupData);
	ISTPopupInternalWidgetInterface::Execute_OnPopupInternalWidgetShow(PopUpInternal,BaseWidget ,PopupData);

	BaseWidget->AddToViewport();

	return true;
}

void ASTHUD::OnPopUpWidgetDestructed(UUserWidget* Widget)
{
	ISTPopupWidgetInterface::Execute_MarkInUse(Widget, false);
}


bool ASTHUD::CloseWidget(FGameplayTag Tag, UObject* CloseData)
{
	if (WidgetStack.Num() <= 0)
		return false;

	if (WidgetStack.Contains(Tag))
	{
		if (WidgetStack.Last() == Tag)
		{
			return CloseLatestWidget();
		}

		UUserWidget* Widget = WidgetBases[Tag];
		ISTWidgetInterface::Execute_OnWidgetRemoved(Widget,CloseData);
		Widget->RemoveFromParent();
		WidgetStack.Remove(Tag);
	}
	else if(WidgetBases.Contains(Tag))
	{
		WidgetBases[Tag]->RemoveFromParent();
		return true;
	}

	if (WidgetStack.Num() == 0 )
	{
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
	}
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
	ISTWidgetInterface::Execute_OnWidgetRemoved(Widget,nullptr);
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
			ShowBlockingWidget(true);

			Widget->AddToViewport();
		}
		ISTWidgetInterface::Execute_OnWidgetOnTop(Widget);
	}
	else
	{
		ShowBlockingWidget(false);
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}

	return true;
}

void ASTHUD::ShowBlockingWidget(bool On)
{
	if (BlockingWidget == nullptr)
		return;

	if (On)
		BlockingWidget->AddToViewport();
	else
		BlockingWidget->RemoveFromParent();
}
