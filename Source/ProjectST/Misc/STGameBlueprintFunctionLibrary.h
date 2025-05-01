// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "STLatentAction.h"
#include "Perception/AISense.h"
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

UENUM(BlueprintType)
enum class EConeTraceSortOption : uint8
{
	None,
	ClosestAngle,
	ClosestDistance
};


class USTGameInstance;


DECLARE_DYNAMIC_DELEGATE(FAsyncTaskDelegate);

UCLASS(Blueprintable)
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

	UFUNCTION(BlueprintCallable)
	static TArray<FHitResult> ConeTraceByChannel(
		AActor* Tracer,
		FVector Start,
		FVector Direction,
		float Distance,
		float ConeAngleDegrees,
		bool bStopTracingOnBlock,
		ETraceTypeQuery TraceChannel,
		EConeTraceSortOption SortOption,
		bool bSortReverse,
		bool bDebug
	);

	UFUNCTION(BlueprintCallable)
	static TArray<FHitResult> ConeTraceByObjectTypes(
		AActor* Tracer,
		FVector Start,
		FVector Direction,
		float Distance,
		float ConeAngleDegrees,
		bool bStopTracingOnBlock,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		EConeTraceSortOption SortOption,
		bool bSortReverse,
		bool bDebug
	);

	static TArray<FHitResult> ConeTrace(
		const TArray<FHitResult>& HitResults, 
		const FVector& Start, 
		const FVector& Direction, 
		float Angle,
		float Distance,
		EConeTraceSortOption SortOption,
		bool bSortReverse,
		bool bDebug
	);


	UFUNCTION(BlueprintPure)
	static FVector CalculateReflectionVector(const FVector& InVector,const FVector& NormalVector,bool bKeepUpDirection = true);


	UFUNCTION(BlueprintCallable)
	static void SetActorStencilValue(AActor* Actor, int32 Value);


	UFUNCTION(BlueprintCallable)
	static bool GetGridPointsFromOffset(TArray<FIntPoint>& OutPoints,TArray<int32>& OutIndices,int32 Index, int32 MaxColCount,int32 MaxRowCount ,const FIntPoint& Size);

	UFUNCTION(BlueprintCallable, meta = (Latent, LatentInfo = "LatentInfo", WorldContext = "WorldContextObject", ExpandEnumAsExecs = "MoveAction", OverTime = "0.2"), Category = "Components")
	static void RotateComponentTo(USceneComponent* Component, FRotator TargetRelativeRotation, bool bEaseOut, bool bEaseIn, float OverTime, bool bForceShortestRotationPath, TEnumAsByte<EMoveComponentAction::Type> MoveAction, FLatentActionInfo LatentInfo);

};
