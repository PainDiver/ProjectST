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