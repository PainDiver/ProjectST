// Fill out your copyright notice in the Description page of Project Settings.


#include "STUserWidget.h"

void USTUserWidget::OnWidgetOnTop_Implementation()
{	
	if (APlayerController* PC = GetOwningPlayer())
	{
		switch (InputMode)
		{
		case EWidgetInputMode::GameAndUI:
		{
			PC->SetInputMode(FInputModeGameAndUI());
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
}
