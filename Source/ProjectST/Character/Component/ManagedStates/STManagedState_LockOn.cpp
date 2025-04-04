// Fill out your copyright notice in the Description page of Project Settings.


#include "STManagedState_LockOn.h"
#include "Game/STNativeGameplayTag.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/STStateHandlingComponent.h"

USTManagedState_LockOn::USTManagedState_LockOn()
{
	TagNotAllowed.AddTag(State_Dead);
}

void USTManagedState_LockOn::OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateAdded_Implementation(StateOwner, OwnerComponent);
	
	OwnerAsCharacter = Cast<ACharacter>(StateOwner);
	if (OwnerAsCharacter == nullptr)
		return;

	PlayerController = Cast<APlayerController>(OwnerAsCharacter->GetController());
	if (PlayerController == nullptr)
		return;

	LockOnTarget = FindLockOnTarget(StateOwner);
	if (LockOnTarget == nullptr)
	{		
		ISTStateInterface::Execute_RemoveState(StateOwner,GetTag());
		OwnerComponent->RemoveManagedState(GetTag());
	}
	else
	{
		OnLockOnTargetSet.Broadcast(LockOnTarget);
	}
}

void USTManagedState_LockOn::OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)
{
	USTManagedState::OnTick_Implementation(StateOwner, OwnerComponent,DeltaTime);

	if (LockOnTarget == nullptr)
	{
		LockOnTarget = FindLockOnTarget(StateOwner);
		if (LockOnTarget == nullptr)
		{
			ISTStateInterface::Execute_RemoveState(StateOwner, GetTag());
			OwnerComponent->RemoveManagedState(GetTag());
			return;
		}
	}
	else
	{
		float Distance = FVector::Distance(LockOnTarget->GetActorLocation(),StateOwner->GetActorLocation());
		if (Distance > LockOnCaptureRadius)
		{
			ISTStateInterface::Execute_RemoveState(StateOwner, GetTag());
			OwnerComponent->RemoveManagedState(GetTag());
			return;
		}

		FVector LookAtVector = (LockOnTarget->GetActorLocation() - PlayerController->PlayerCameraManager->GetCameraLocation()).GetSafeNormal();
		TargetRotation = LookAtVector.Rotation();		
		FRotator InterpolatedRot = FMath::RInterpTo(PlayerController->GetControlRotation(),TargetRotation,DeltaTime, InterpSpeed);
		PlayerController->SetControlRotation(InterpolatedRot);
	}
}

void USTManagedState_LockOn::OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)
{
	USTManagedState::OnStateRemoved_Implementation(StateOwner,OwnerComponent);

	if (LockOnTarget)
	{
		LockOnTarget = nullptr;
	}

	OnLockOnTargetRemoved.Broadcast();
	// Remove LockOnTarget
}

AActor* USTManagedState_LockOn::FindLockOnTarget(AActor* StateOwner)
{
	TArray<FHitResult> HitResults;
	TArray<AActor*> IgnoredActors{ StateOwner };
	FVector OwnerLocation = StateOwner->GetActorLocation();
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		OwnerLocation,
		OwnerLocation,
		LockOnCaptureRadius,
		LockOnTargetTypes,
		false,
		IgnoredActors,
		EDrawDebugTrace::None,
		HitResults,
		true);

	AActor* ClosestActor = nullptr;

	if (HitResults.Num() < 0)
	{
		ISTStateInterface::Execute_RemoveState(StateOwner, CombatState_LockOn);
	}
	else
	{
		FVector ControlVector = OwnerAsCharacter->GetControlRotation().Vector().GetSafeNormal();
		float ClosestAngle = FLT_MAX;
		TSet<AActor*> Processed;
		float Angle;
		FVector OwnerToTarget;
		for (const FHitResult& HitResult : HitResults)
		{
			if (Processed.Contains(HitResult.GetActor()))
			{
				continue;
			}
			Processed.Add(HitResult.GetActor());

			OwnerToTarget = (HitResult.GetActor()->GetActorLocation() - PlayerController->PlayerCameraManager->GetCameraLocation()).GetSafeNormal();

			Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(ControlVector, OwnerToTarget)));
			if (Angle > LockOnCaptureMaxAngle)
			{
				continue;
			}

			if (ClosestAngle > Angle)
			{
				ClosestAngle = Angle;
				ClosestActor = HitResult.GetActor();
			}
		}
	}

	return ClosestActor;
}
