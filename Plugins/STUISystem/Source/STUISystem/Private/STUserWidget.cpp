// Fill out your copyright notice in the Description page of Project Settings.


#include "STUserWidget.h"

void USTUserWidget::OnWidgetOnTop_Implementation()
{	
	AdjustInputMode();
	if (bDisableCharacterInput && GetOwningPlayerPawn())
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void USTUserWidget::OnWidgetShow_Implementation(UObject* OpenData)
{
	AdjustInputMode();
	if (bDisableCharacterInput && GetOwningPlayerPawn())
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}

void USTUserWidget::OnWidgetRemoved_Implementation(UObject* OpenData)
{
	if (bDisableCharacterInput && GetOwningPlayerPawn())
	{
		GetOwningPlayerPawn()->EnableInput(GetOwningPlayer());
	}
}

void USTUserWidget::SetWidgetController(EWidgetControllerType Type,TObjectPtr<USTWidgetController> NewController)
{
	WidgetController.Add(Type, NewController);
}

void USTUserWidget::AdjustInputMode()
{
	if (APlayerController* PC = GetOwningPlayer())
	{
		switch (InputMode)
		{
		case EWidgetInputMode::GameAndUI:
		{
			FInputModeGameAndUI NewInputMode;
			NewInputMode.SetWidgetToFocus(GetAccessibleWidget());
			NewInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			NewInputMode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(NewInputMode);
			break;
		}
		case EWidgetInputMode::UI:
		{
			PC->SetInputMode(FInputModeUIOnly());
			break;
		}
		case EWidgetInputMode::GameOnly:
		{
			PC->SetInputMode(FInputModeGameOnly());
			break;
		}
		}

		PC->bShowMouseCursor = bShowMouseCursor;
	}

	if (bDisableCharacterInput && GetOwningPlayerPawn())
	{
		GetOwningPlayerPawn()->DisableInput(GetOwningPlayer());
	}
}
