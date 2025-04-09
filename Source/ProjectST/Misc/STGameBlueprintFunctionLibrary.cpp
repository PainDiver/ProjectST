// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STGameBlueprintFunctionLibrary.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"

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

