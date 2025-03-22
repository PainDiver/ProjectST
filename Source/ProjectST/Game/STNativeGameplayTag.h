// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"


// State
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Standing)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Falling)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Lying)

// HitType
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_Head)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_Right)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_Left)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_Flying)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_KnockDown)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Hit_Body)


// KnockbackType
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Knockback_Pull)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Knockback_PullToHitter)
UE_DECLARE_GAMEPLAY_TAG_EXTERN(Knockback_Push)
