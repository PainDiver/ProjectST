// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWidgetModalType : uint8
{
	Modal,
	Modeless
};

UENUM(BlueprintType)
enum class EWidgetInputMode : uint8
{
	GameAndUI,
	UI,
	GameOnly
};
