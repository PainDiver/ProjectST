// Fill out your copyright notice in the Description page of Project Settings.


#include "STPopupInternalUserWidget.h"
#include "STPopupUserWidget.h"


USTPopupUserWidget* USTPopupInternalUserWidget::GetParentUserWidget()
{
	return GetTypedOuter<USTPopupUserWidget>();
}