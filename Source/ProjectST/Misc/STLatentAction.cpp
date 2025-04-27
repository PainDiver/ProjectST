// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STLatentAction.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Misc/STSplinePack.h"

void FSplineMoveToAction::UpdateOperation(FLatentResponse& Response)
{
	if (bShouldEnd == true)
	{
		if (SplinePack)
		{
			SplinePack->RemoveFromRoot();
			SplinePack = nullptr;
		}

		MovementComp->Velocity = FVector::ZeroVector;
		if (InputType == EMoveToInputType::Block && Target->GetController())
		{
			Target->GetController()->SetIgnoreMoveInput(false);
		}

		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}
	// Update elapsed time
	TimeElapsed += Response.ElapsedTime();

	bool bComplete = (TimeElapsed >= TotalTime);

	// If we have a component to modify..
	if (Target.IsValid() && bInterpolating)
	{
		if (!bOnceFired)
		{
			if (InputType == EMoveToInputType::Block && Target->GetController())
			{
				Target->GetController()->SetIgnoreMoveInput(true);
			}
			bOnceFired = true;
		}

		// Work out 'Blend Percentage'
		const float BlendExp = 2.f;
		float DurationPct = TimeElapsed / TotalTime;
		float BlendPct;
		if (bEaseIn)
		{
			if (bEaseOut)
			{
				// EASE IN/OUT
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, BlendExp);
			}
			else
			{
				// EASE IN
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, BlendExp));
			}
		}
		else
		{
			if (bEaseOut)
			{
				// EASE OUT
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / BlendExp));
			}
			else
			{
				// LINEAR
				BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
			}
		}

		FInterpCurvePoint<FVector> LastPoint = SplinePack->SplineCurves.Position.Points.Last();
		float MaxKey = LastPoint.InVal;
		float ProgressRatio = MaxKey * BlendPct;

		ESplineCoordinateSpace::Type CoordinateType = ESplineCoordinateSpace::World;

		FTransform NewTransform = SplinePack->GetTransformAtSplineInputKey
		(
			InitialTransform,
			Target->GetActorLocation(),
			FocalActor.IsValid() ? FocalActor->GetActorLocation() : FocalPoint,
			ProgressRatio,
			CoordinateType,
			true
		);

		if (!bIsVelocityBase)
		{
			Target->SetActorTransform(NewTransform, false);
		}
		else if(MovementComp.IsValid())
		{			
			DrawDebugSphere(Target->GetWorld(), NewTransform.GetLocation(), 50.f, 12, FColor::Red, false, 5.f);
			FVector Delta = (NewTransform.GetLocation() - LastTransform.GetLocation()) / Response.ElapsedTime();						
			MovementComp->Velocity = Delta.GetClampedToSize(0.f,2000.f);
			Target->SetActorRotation(NewTransform.Rotator());
		}

		LastTransform = Target->GetActorTransform();
	}

	if (bComplete)
	{
		// SplinePack 일회성으로 만들거라서 GC에 의해 업데이트 도중 날아갈수도있음..
		// 그래서 일단 이거 Call하는곳에서 Curve동적으로 만들고 Root에 박아뒀다가 
		// 다쓰면 제거하는방식으로 해야할듯
		if (SplinePack)
		{
			SplinePack->RemoveFromRoot();
			SplinePack = nullptr;
		}

		MovementComp->Velocity = FVector::ZeroVector;
		if (InputType == EMoveToInputType::Block && Target->GetController())
		{
			Target->GetController()->SetIgnoreMoveInput(false);
		}
	}


	Response.FinishAndTriggerIf(bComplete || !bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);

}

void FParabolicMoveToAction::UpdateOperation(FLatentResponse& Response)
{
	if (bShouldEnd == true)
	{
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		Character->GetController()->SetIgnoreMoveInput(false);
		MovementComp->BrakingDecelerationFalling = InitialBrakingDeceleration;
		MovementComp->bIgnoreClientMovementErrorChecksAndCorrection = false;

		return;
	}

	bool bComplete = false;

	// If we have a component to modify..
	if (Character.IsValid())
	{
		TimeElapsed += Response.ElapsedTime();

		if (bOnceFired == false)
		{
			MovementComp = Character->GetCharacterMovement();
			float Distance = FVector::Distance(StartLocation, TargetLocation);
			float LaunchSpeed = Distance * LaunchSpeedScale;

			UGameplayStatics::FSuggestProjectileVelocityParameters ProjectileParams = UGameplayStatics::FSuggestProjectileVelocityParameters(Character.Get(), Character->GetActorLocation(), TargetLocation, LaunchSpeed);
			ProjectileParams.bFavorHighArc = bFavorHighArc;
			ProjectileParams.CollisionRadius = 0.f;			
			ProjectileParams.TraceOption = CollisionType;
			ProjectileParams.OverrideGravityZ = MovementComp->GetGravityZ();
			ProjectileParams.bDrawDebug = bDebugPath;
			ProjectileParams.bAcceptClosestOnNoSolutions = bAcceptClosestOnNoSolutions;

			FVector OutVelocity;
			if (UGameplayStatics::SuggestProjectileVelocity(ProjectileParams, OutVelocity))
			{
				InitialBrakingDeceleration = MovementComp->BrakingDecelerationFalling;
				MovementComp->BrakingDecelerationFalling = 0.f;

				if (Character->GetController())
				{
					if (InputType == EParabolicMoveToInputType::Block || InputType == EParabolicMoveToInputType::BlockAscending)
					{
						Character->GetController()->SetIgnoreMoveInput(true);
					}
				}
				MovementComp->bIgnoreClientMovementErrorChecksAndCorrection = true;
				Character->SetActorLocation(StartLocation);
				Character->LaunchCharacter(OutVelocity, true, true);				
				bOnceFired = true;
			}
			else
			{
				*Result = EParabolicMoveToResult::Fail;
				bComplete = true;
			}
		}
		else if (MovementComp->IsFalling())
		{			

			if (Character->GetVelocity().Z < 0.f && InputType == EParabolicMoveToInputType::BlockAscending)
			{
				Character->GetController()->SetIgnoreMoveInput(false);
			}			
		}
		else if(TimeElapsed > 0.2f)
		{			
			*Result = EParabolicMoveToResult::OnLand;
			bComplete = true;			
		}
	}
	else
	{		
		*Result = EParabolicMoveToResult::Fail;
		bComplete = true;
	}

	if (bComplete && bOnceFired && MovementComp.IsValid())
	{
		Character->GetController()->SetIgnoreMoveInput(false);
		MovementComp->BrakingDecelerationFalling = InitialBrakingDeceleration;
		MovementComp->bIgnoreClientMovementErrorChecksAndCorrection = false;

	}

	Response.FinishAndTriggerIf(bComplete, ExecutionFunction, OutputLink, CallbackTarget);
}

void FInterpolateRotationOfComponentToAction::UpdateOperation(FLatentResponse& Response)
{	
	if (bShouldEnd == true)
	{
		Character->GetController()->SetIgnoreMoveInput(false);
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}

	// Update elapsed time
	TimeElapsed += Response.ElapsedTime();

	bool bComplete = (TimeElapsed >= TotalTime);

	// If we have a component to modify..
	if (TargetComponent.IsValid() && bInterpolating)
	{
		if (bOnceFired == false && Character.IsValid() && Character->GetController())
		{
			Character->GetController()->SetIgnoreMoveInput(true);
			bOnceFired = true;
		}

		// Work out 'Blend Percentage'
		const float BlendExp = 2.f;
		float DurationPct = TimeElapsed / TotalTime;
		float BlendPct;
		if (bEaseIn)
		{
			if (bEaseOut)
			{
				// EASE IN/OUT
				BlendPct = FMath::InterpEaseInOut(0.f, 1.f, DurationPct, BlendExp);
			}
			else
			{
				// EASE IN
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, BlendExp));
			}
		}
		else
		{
			if (bEaseOut)
			{
				// EASE OUT
				BlendPct = FMath::Lerp(0.f, 1.f, FMath::Pow(DurationPct, 1.f / BlendExp));
			}
			else
			{
				// LINEAR
				BlendPct = FMath::Lerp(0.f, 1.f, DurationPct);
			}
		}

		if (bInterpRotation && TargetComponent.IsValid())
		{
			FRotator NewRotation;
			// If we are done just set the final rotation
			if (bComplete)
			{
				NewRotation = TargetRotation;
			}
			else if (bForceShortestRotationPath)
			{
				// We want the shortest path 
				FQuat AQuat(InitialRotation);
				FQuat BQuat(TargetRotation);

				FQuat Result = FQuat::Slerp(AQuat, BQuat, BlendPct);
				Result.Normalize();
				NewRotation = Result.Rotator();
			}
			else
			{
				// dont care about it being the shortest path - just lerp
				NewRotation = FMath::Lerp(InitialRotation, TargetRotation, BlendPct);
			}
			TargetComponent->SetRelativeRotation(NewRotation, false);
		}
	}

	if (bComplete)
	{
		Character->GetController()->SetIgnoreMoveInput(false);
	}
	Response.FinishAndTriggerIf(bComplete || !bInterpolating, ExecutionFunction, OutputLink, CallbackTarget);
	
}
