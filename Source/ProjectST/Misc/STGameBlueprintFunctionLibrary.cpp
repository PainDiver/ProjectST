// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STGameBlueprintFunctionLibrary.h"

bool USTGameBlueprintFunctionLibrary::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}