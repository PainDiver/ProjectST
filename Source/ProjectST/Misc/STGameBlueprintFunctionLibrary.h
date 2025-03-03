// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "STGameBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
class USTGameInstance;

DECLARE_DYNAMIC_DELEGATE(FAsyncTaskDelegate);

UCLASS()
class PROJECTST_API USTGameBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	static bool IsEditor();
	
	UFUNCTION(BlueprintCallable)
	static void DispatchAsyncTask(FAsyncTaskDelegate Delegate);

	UFUNCTION(BlueprintCallable)
	static USTGameInstance* GetSTGameInstance();
};
