// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


UENUM(Blueprintable)
enum class EComboContextState :uint8
{
	DEFAULT,
	JUMPING,
	ON_HIT,
	MAX
};


UENUM(Blueprintable)
enum class EItemUseType :uint8
{
	NONE,
	CONSUME,
	EQUIP,
	CONTAINER
};

UENUM(Blueprintable)
enum class EItemContainerType :uint8
{
	EQUIPMENT,
	INVENTORY
};

UENUM(Blueprintable)
enum class EEquipSlotType :uint8
{
	NONE = 0,
	
	HEAD,
	ARMOR,
	PANTS,
	WEAPON,
	MAX
};
