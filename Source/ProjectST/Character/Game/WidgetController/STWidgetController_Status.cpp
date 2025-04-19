// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Game/WidgetController/STWidgetController_Status.h"
#include "Character/Component/STInventoryComponent.h"
#include "Character/STCharacterInterface.h"

USTWidgetController_Status::USTWidgetController_Status()
{
	ControllerType = EWidgetControllerType::StatusBaseController;
}

void USTWidgetController_Status::OnWidgetControllerSet(APlayerController* Owner)
{	
	InventoryComp = ISTCharacterInterface::Execute_GetInventoryComponent(Owner->GetPawn());
}
