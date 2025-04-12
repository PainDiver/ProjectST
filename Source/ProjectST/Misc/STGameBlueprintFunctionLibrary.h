// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "SequenceEvaluatorLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "STLatentAction.h"
#include "STGameBlueprintFunctionLibrary.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSTSplineCurveKey
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Time;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FVector Value;
};


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

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", OverTime = "0.2"), Category = "Components")
	static void SplineMoveTo(
		ACharacter* Target,
		const TArray<FSTSplineCurveKey>& SplinePoints,
		TSubclassOf<USTSplinePack> SplinePackIfExists,
		bool bIsSplineWorldPos,
		FRotator TargetRelativeRotation,
		const FVector& FocalPoint,
		AActor* FocalActor,
		bool bEaseOut,
		bool bEaseIn,
		float OverTime,
		bool bIsVelocityBase,
		EMoveToInputType InputType,
		FLatentActionInfo LatentInfo);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo",LaunchSpeedScale = "1.5", ExpandEnumAsExecs = "Result"), Category = "Components")
	static void ParabolicMoveTo(
		ACharacter* Character,
		const FVector& StartLoc,
		const FVector& TargetLoc,
		float LaunchSpeedScale ,
		bool bFavorHighArc, 
		EParabolicMoveToInputType InputType,
		TEnumAsByte<ESuggestProjVelocityTraceOption::Type> TraceType, 
		float CollisionRadius, 
		bool bAcceptClosestOnNoSolutions, 
		bool bDebug, 
		EParabolicMoveToResult& Result,
		FLatentActionInfo LatentInfo);


};
