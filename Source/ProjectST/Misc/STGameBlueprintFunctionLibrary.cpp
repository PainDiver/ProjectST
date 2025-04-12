// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STGameBlueprintFunctionLibrary.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Misc/STSplinePack.h"

bool USTGameBlueprintFunctionLibrary::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

void USTGameBlueprintFunctionLibrary::DispatchAsyncTask(FAsyncTaskDelegate Delegate)
{
	AsyncTask(ENamedThreads::Type::AnyThread,
		[Task = MoveTemp(Delegate)]()
		{
			Task.ExecuteIfBound();
		});
}

USTGameInstance* USTGameBlueprintFunctionLibrary::GetSTGameInstance()
{
	if(GEngine)
		return Cast<USTGameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld()));

	return nullptr;
}

void USTGameBlueprintFunctionLibrary::RequestExit(bool bForce, FString CallSite)
{
	FPlatformMisc::RequestExit(bForce,*CallSite);
}

FGameplayTag USTGameBlueprintFunctionLibrary::RequestGameplayTag(FName TagName)
{
	return FGameplayTag::RequestGameplayTag(TagName, false);
}

UObject* USTGameBlueprintFunctionLibrary::GetDefaultObject(TSubclassOf<UObject> ObjectClass)
{
	if (ObjectClass)
	{
		return ObjectClass->GetDefaultObject();
	}

	return nullptr;
}

bool USTGameBlueprintFunctionLibrary::GetCurveTimeByValue(const UAnimSequenceBase* Animation, FName CurveName, float Value, float& OutValue)
{
	OutValue = 0.0f;
	if (Animation && Animation->HasCurveData(CurveName))
	{
		if (const FFloatCurve* Curve = (const FFloatCurve*)Animation->GetCurveData().GetCurveData(CurveName, ERawCurveTrackTypes::RCT_Float))
		{
			int ClosestIndex = 0;
			float ClosestDiff = MAX_FLT;
			for (int i=0;i< Curve->FloatCurve.Keys.Num(); i++ )
			{
				float Diff = abs(Curve->FloatCurve.Keys[i].Value -  Value);				
				if (ClosestDiff > Diff)
				{
					ClosestDiff = Diff;
					ClosestIndex = i;
				}
			}

			bool bIsPrevKeyAlive = Curve->FloatCurve.Keys.IsValidIndex(ClosestIndex - 1);
			bool bIsNextKeyAlive = Curve->FloatCurve.Keys.IsValidIndex(ClosestIndex + 1);

			float ClosestValue = Curve->FloatCurve.Keys[ClosestIndex].Value;
			float SecondClosestValue = 0.f;
			bool bIsSecondClosestValueBigger = false;
			if (bIsPrevKeyAlive && bIsNextKeyAlive)
			{
				ClosestValue = Curve->FloatCurve.Keys[ClosestIndex].Value;
				float PrevValue = Curve->FloatCurve.Keys[ClosestIndex - 1].Value;
				float NextValue = Curve->FloatCurve.Keys[ClosestIndex + 1].Value;
				
				float PrevDiff = abs(ClosestValue - PrevValue);
				float NextDiff = abs(ClosestValue - NextValue);

				if (PrevDiff > NextDiff)
				{
					SecondClosestValue = NextValue;
					bIsSecondClosestValueBigger = true;
				}
				else
				{
					SecondClosestValue = PrevValue;
					bIsSecondClosestValueBigger = false;
				}
			}
			else if (!bIsPrevKeyAlive && bIsNextKeyAlive)
			{
				ClosestValue = Curve->FloatCurve.Keys[ClosestIndex].Value;
				SecondClosestValue = Curve->FloatCurve.Keys[ClosestIndex + 1].Value;
				bIsSecondClosestValueBigger = true;
			}
			else if (bIsPrevKeyAlive && !bIsNextKeyAlive)
			{
				ClosestValue = Curve->FloatCurve.Keys[ClosestIndex].Value;
				SecondClosestValue = Curve->FloatCurve.Keys[ClosestIndex-1].Value;
				bIsSecondClosestValueBigger = false;
			}
			else
			{
				OutValue = Curve->FloatCurve.Keys[ClosestIndex].Time;
				return true;
			}
			float LerpRatio = Value / (ClosestValue + SecondClosestValue);

			float ClosestKeyTime = Curve->FloatCurve.Keys[ClosestIndex].Time;
			float SecondClosestKeyTime = 0.f;
			if (bIsSecondClosestValueBigger)
			{
				SecondClosestKeyTime = Curve->FloatCurve.Keys[ClosestIndex + 1].Time;
				OutValue = FMath::Lerp(ClosestKeyTime, SecondClosestKeyTime, LerpRatio);
			}
			else
			{
				SecondClosestKeyTime = Curve->FloatCurve.Keys[ClosestIndex - 1].Time;
				OutValue = FMath::Lerp(SecondClosestKeyTime, ClosestKeyTime, LerpRatio);
			}


			return true;
		}
	}
	return false;
}

void USTGameBlueprintFunctionLibrary::SplineMoveTo(
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
	FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(Target, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		FSplineMoveToAction* Action = LatentActionManager.FindExistingAction<FSplineMoveToAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
		
		if (Action)
		{

		}
		else
		{
			USTSplinePack* SplinePack = nullptr;
			if (SplinePackIfExists == nullptr)
			{
				SplinePack = NewObject<USTSplinePack>(Target);
				SplinePack->AddToRoot();
				for (const FSTSplineCurveKey& Point : SplinePoints)
				{
					float Time = Point.Time;
					FVector Value = Point.Value;
					for (int32 Axis = 0; Axis < 3; ++Axis)
					{
						SplinePack->SplineCurves.Position.AddPoint(Time, Value);
						SplinePack->SplineCurves.Rotation.AddPoint(Time, FQuat::Identity);
						SplinePack->SplineCurves.Scale.AddPoint(Time, FVector::OneVector);
					}
					SplinePack->SplineCurves.UpdateSpline();
				}
			}
			else
			{
				SplinePack = Cast<USTSplinePack>(SplinePackIfExists->GetDefaultObject());
			}
			
			// Only act on a 'move' input if not running
			Action = new FSplineMoveToAction(OverTime, LatentInfo, Target, bEaseOut, bEaseIn, SplinePack);
			Action->bIsSplineWorldPos = bIsSplineWorldPos;
			Action->FocalPoint = FocalPoint;
			Action->bIsVelocityBase = bIsVelocityBase;
			Action->InitialTransform = Target->GetActorTransform();
			Action->LastTransform = Target->GetActorTransform();
			Action->FocalActor = FocalActor;
			Action->MovementComp = Target->GetComponentByClass<UCharacterMovementComponent>();
			Action->InputType = InputType;

			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
		}
	}
}

void USTGameBlueprintFunctionLibrary::ParabolicMoveTo(
	ACharacter* Character, 
	const FVector& StartLoc,
	const FVector& TargetLoc, 
	float LaunchSpeedScale,
	bool bFavorHighArc, 
	EParabolicMoveToInputType InputType,
	TEnumAsByte<ESuggestProjVelocityTraceOption::Type> TraceType, 
	float CollisionRadius, 
	bool bAcceptClosestOnNoSolutions, 
	bool bDebug,EParabolicMoveToResult& Result, 
	FLatentActionInfo LatentInfo)
{
	if (UWorld* World = GEngine->GetWorldFromContextObject(Character, EGetWorldErrorMode::LogAndReturnNull))
	{
		FLatentActionManager& LatentActionManager = World->GetLatentActionManager();
		FParabolicMoveToAction* Action = LatentActionManager.FindExistingAction<FParabolicMoveToAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
		if (Action)
		{
		
		}
		else
		{
			Action = new FParabolicMoveToAction(LatentInfo, Character);
			Action->StartLocation = StartLoc;
			Action->TargetLocation = TargetLoc;
			Action->LaunchSpeedScale = LaunchSpeedScale;
			Action->CollisionType = TraceType;
			Action->CollisionRadius = CollisionRadius;
			Action->bFavorHighArc = bFavorHighArc;
			Action->InputType = InputType;
			Action->bAcceptClosestOnNoSolutions = bAcceptClosestOnNoSolutions;
			Action->bDebugPath = bDebug;
			Action->Result = &Result;

			LatentActionManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, Action);
		}
	}
}

