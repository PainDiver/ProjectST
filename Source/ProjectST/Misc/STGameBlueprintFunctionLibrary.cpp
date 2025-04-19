// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STGameBlueprintFunctionLibrary.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveVector.h"
#include "Misc/STSplinePack.h"
#include "KismetTraceUtils.h"
#include "Kismet/KismetMathLibrary.h"

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

TArray<FHitResult> USTGameBlueprintFunctionLibrary::ConeTraceByChannel(
	AActor* Tracer,
	FVector Start,
	FVector Direction,
	float Distance,
	float ConeAngleDegrees,
	bool bStopTracingOnBlock,
	ETraceTypeQuery TraceChannel,
	EConeTraceSortOption SortOption,
	bool bSortReverse,
	bool bDebug)
{
	if (!Tracer) 
		return TArray<FHitResult>();

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(Tracer);
	if (!World) 
		return TArray<FHitResult>();

	FVector End = Start + Direction * Distance;
	float Radius = Distance * FMath::Tan(FMath::DegreesToRadians(ConeAngleDegrees));
	TArray<FHitResult> HitResults;	
	TArray<AActor*> ActorsToIgnore{Tracer};
	bool bHit =UKismetSystemLibrary::SphereTraceMulti
	(
		Tracer,
		Start,
		End,
		Radius,
		TraceChannel,
		false,
		ActorsToIgnore,
		bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.2f
	);

#if WITH_EDITOR
	if (bDebug)
	{
		DrawDebugCone(Tracer->GetWorld(), Start, Direction, Distance, FMath::DegreesToRadians(ConeAngleDegrees), FMath::DegreesToRadians(ConeAngleDegrees), 12, FColor::White, false, 0.f);
	}
#endif

	TArray<FHitResult> Result;
	if (bHit)
	{
		return ConeTrace(
			HitResults,
			Start,
			Direction,
			ConeAngleDegrees,
			Distance,
			SortOption,
			bSortReverse,
			bDebug
		);
	}

	return TArray<FHitResult>();

}

TArray<FHitResult> USTGameBlueprintFunctionLibrary::ConeTraceByObjectTypes(
	AActor* Tracer,
	FVector Start,
	FVector Direction,
	float Distance, 
	float ConeAngleDegrees,
	bool bStopTracingOnBlock,
	const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	EConeTraceSortOption SortOption,
	bool bSortReverse,
	bool bDebug)
{

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(Tracer);
	if (!World) 
		return TArray<FHitResult>();

	FVector End = Start + Direction * Distance;
	float Radius = Distance * FMath::Tan(FMath::DegreesToRadians(ConeAngleDegrees));
	TArray<FHitResult> HitResults;	
	TArray<AActor*> ActorsToIgnore;
	bool bHit = UKismetSystemLibrary::SphereTraceMultiForObjects
	(
		Tracer,
		Start,
		End,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		bDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.2f
	);

#if WITH_EDITOR
	if (bDebug)
	{
		DrawDebugCone(Tracer->GetWorld(), Start, Direction, Distance, FMath::DegreesToRadians(ConeAngleDegrees), FMath::DegreesToRadians(ConeAngleDegrees), 12, FColor::White, false, 0.f);
	}
#endif
	if (bHit)
	{
		return ConeTrace(
			HitResults,
			Start,
			Direction,
			ConeAngleDegrees,
			Distance,
			SortOption,
			bSortReverse,
			bDebug
		);
	}

	return TArray<FHitResult>();
}

TArray<FHitResult> USTGameBlueprintFunctionLibrary::ConeTrace(
	const TArray<FHitResult>& HitResults,
	const FVector& Start,
	const FVector& Direction ,
	float Angle,
	float Distance, 
	EConeTraceSortOption SortOption,
	bool bSortReverse,
	bool bDebug)
{	
	float CosConeAngle = FMath::Cos(FMath::DegreesToRadians(Angle));

	TSet<AActor*> ProcessedActors;
	TArray<FHitResult> Res;
	TFunction<bool(const FHitResult&, const FHitResult&)> Predicate;
	
	switch (SortOption)
	{
		case EConeTraceSortOption::ClosestAngle:
		{
			Predicate = [bSortReverse,Direction](const FHitResult& A, const FHitResult& B)
				{					
					FVector DirectionToA = (A.ImpactPoint - A.TraceStart).GetSafeNormal();
					FVector DirectionToB = (B.ImpactPoint - A.TraceStart).GetSafeNormal();
					float ADot = FVector::DotProduct(Direction, DirectionToA);
					float BDot = FVector::DotProduct(Direction, DirectionToB);
					return bSortReverse? (ADot < BDot) : (ADot > BDot);
				};
			break;
		}
		case EConeTraceSortOption::ClosestDistance:
		{
			Predicate = [bSortReverse](const FHitResult& A, const FHitResult& B)
				{
					float ADist = FVector::Distance(A.TraceStart, A.ImpactPoint);
					float BDist = FVector::Distance(A.TraceStart, B.ImpactPoint);
					return bSortReverse ? (ADist > BDist) : (ADist < BDist);
				};
			break;
		}
		default:
			break;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
			continue;

		if (ProcessedActors.Contains(HitActor))
			continue;

		FVector ToActor = Hit.ImpactPoint - Start;
		float DistanceToActor = ToActor.Size();
		if (DistanceToActor > Distance)
			continue;

		FVector DirectionToActor = ToActor.GetSafeNormal();
		float Dot = FVector::DotProduct(Direction, DirectionToActor);
		if (Dot >= CosConeAngle)
		{			
			if (SortOption != EConeTraceSortOption::None)
			{
				Res.HeapPush(Hit, Predicate);
			}
			else
			{
				Res.Add(Hit);
			}
			ProcessedActors.Add(HitActor);			
		}
	}


#if WITH_EDITOR
	if (bDebug)
	{
		for (int i=0;i<Res.Num();i++)
		{
			DrawDebugSphere(Res[i].GetActor()->GetWorld(), Res[i].ImpactPoint, (i+1)*10.f , 5, FColor::Red, false, 0.1f, 0, 0.2f);
		}
	}
#endif


	return MoveTemp(Res);
}

FVector USTGameBlueprintFunctionLibrary::CalculateReflectionVector(const FVector& InVector, const FVector& NormalVector, bool bKeepUpDirection)
{
	FVector NormalizedVector = InVector.GetSafeNormal();
	float Scale = bKeepUpDirection ? 2.f : 1.f;
	return NormalizedVector + (Scale * FVector::DotProduct(-NormalizedVector, NormalVector) * NormalVector);
}

void USTGameBlueprintFunctionLibrary::SetActorStencilValue(AActor* Actor, int32 Value)
{
	if (Actor == nullptr)
		return;

	TArray<UMeshComponent*> Meshes;
	Actor->GetComponents<UMeshComponent>(Meshes);
	for (UMeshComponent* Mesh : Meshes)
	{
		Mesh->SetCustomDepthStencilValue(Value);
	}
}

bool USTGameBlueprintFunctionLibrary::GetGridPointsFromOffset(TArray<FIntPoint>& OutPoints,TArray<int32>& OutIndices, int32 Index, int32 MaxColCount, int32 MaxRowCount, const FIntPoint& Size)
{
	for (int row = 0; row < Size.Y; row++)
	{
		for (int col = 0; col < Size.X; col++)
		{
			int32 Index1D = Index + col + (MaxColCount * row);
			if (col != 0 && (Index1D % MaxColCount == 0))
			{
				return false;
			}
			else if ( Index1D/MaxColCount > MaxRowCount)
			{
				return false;
			}
			OutIndices.Add(Index1D);
			OutPoints.Add(UKismetMathLibrary::Convert1DTo2D(Index1D, MaxColCount));
		}
	}

	return true;
}
