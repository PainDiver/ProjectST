// Fill out your copyright notice in the Description page of Project Settings.


#include "Params.h"

bool UHitParam::IsValidHitParam()
{
	return !TargetState.IsEmpty();
}
