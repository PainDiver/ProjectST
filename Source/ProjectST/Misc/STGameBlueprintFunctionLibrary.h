// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "SequenceEvaluatorLibrary.h"

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

	UFUNCTION(BlueprintCallable)
	static void RequestExit(bool bForce, FString CallSite);

	UFUNCTION(BlueprintPure)
	static FGameplayTag RequestGameplayTag(FName TagName);

	UFUNCTION(BlueprintPure)
	static UObject* GetDefaultObject(TSubclassOf<UObject> ObjectClass);

	UFUNCTION(BlueprintPure, Category = "Distance Matching", meta = (BlueprintThreadSafe))
	static bool GetCurveTimeByValue(const UAnimSequenceBase* Animation, FName CurveName, float Value, float& OutValue);
};
