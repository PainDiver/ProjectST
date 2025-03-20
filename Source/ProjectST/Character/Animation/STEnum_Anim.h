// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class ETargetQueryType: uint8
{
	Box,
	Sphere,
	Capsule
};