// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_CameraSpline.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/SavePackage.h"
#include "Misc/STSplinePack.h"



void UANS_CameraSpline::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);

	if (APawn* Pawn = Cast<APawn>(MeshComp->GetOwner()))
	{
		if (!Pawn->IsLocallyControlled())
		{
			return;
		}
	}

	UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
	if (ScratchPad == nullptr || SplinePack == nullptr)
	{
		return;
	}

	ScratchPad->Owner = MeshComp->GetOwner();	
	ScratchPad->CameraComponent = ScratchPad->Owner->GetComponentByClass<UCameraComponent>();	 	
	ScratchPad->SpringArmComponent = ScratchPad->Owner->GetComponentByClass<USpringArmComponent>();
	ScratchPad->SplinePack = Cast<USTSplinePack>(SplinePack->GetDefaultObject());
	if (ScratchPad->Owner == nullptr ||
		ScratchPad->CameraComponent == nullptr ||
		ScratchPad->SpringArmComponent == nullptr)
	{
		return;
	}

	FDetachmentTransformRules DetachmentRule{EDetachmentRule::KeepWorld,false};
	ScratchPad->CameraComponent->DetachFromComponent(DetachmentRule);
}

void UANS_CameraSpline::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);

	if (SplinePack == nullptr)
		return;

	UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
	if (ScratchPad == nullptr || ScratchPad->Owner == nullptr|| ScratchPad->CameraComponent == nullptr || ScratchPad->SplinePack->SplineCurves.Position.Points.Num() ==0)
	{
		return;
	}	

	ScratchPad->ElapsedTime += FrameDeltaTime;

	float SplineDuration = ScratchPad->SplinePack->SplineCurves.Position.Points.Last().InVal;
	float NomalizingRatio = SplineDuration / EventReference.GetNotify()->Duration;

	float Progress = ScratchPad->ElapsedTime * NomalizingRatio;

	FTransform Transform = ScratchPad->CameraComponent->GetComponentTransform();	

	float RemainingRatio = 1.f - (Progress / SplineDuration);
	
	FTransform InterpedTransform;
	if (bEndBlend &&  RemainingRatio <= EndBlendRatio)
	{
		float LerpRatio = (EndBlendRatio - RemainingRatio) / 1.f;
		FTransform SocketTransform = ScratchPad->SpringArmComponent->GetSocketTransform("",ERelativeTransformSpace::RTS_World);
		FVector NewLoc = FMath::Lerp<FVector>(Transform.GetLocation(), SocketTransform.GetLocation(), LerpRatio);
		FRotator NewRot = FMath::Lerp<FRotator>(Transform.GetRotation().Rotator(), SocketTransform.GetRotation().Rotator(), LerpRatio);
		InterpedTransform = FTransform{ NewRot,NewLoc,FVector::OneVector };
	}
	else
	{
		FTransform NewTransform = ScratchPad->SplinePack->GetTransformAtSplineInputKey(ScratchPad->Owner->GetActorTransform(), ScratchPad->CameraComponent->GetComponentLocation(), ScratchPad->Owner->GetActorLocation(), Progress, ESplineCoordinateSpace::World, false);
		FVector NewLoc = FMath::VInterpTo(Transform.GetLocation(), NewTransform.GetLocation(), FrameDeltaTime, ScratchPad->SplinePack->InterpSpeed);
		FRotator NewRot = FMath::RInterpTo(Transform.GetRotation().Rotator(), NewTransform.GetRotation().Rotator(), FrameDeltaTime, ScratchPad->SplinePack->InterpSpeed);
		InterpedTransform = FTransform{ NewRot,NewLoc,FVector::OneVector };
	}
	ScratchPad->CameraComponent->SetWorldTransform(InterpedTransform);


#if WITH_EDITORONLY_DATA
	if (bShowDebug)
	{
		DrawDebugSphere(
			MeshComp->GetWorld(),
			ScratchPad->CameraComponent->GetComponentLocation(),
			25.f,
			5,
			FColor::Red,
			false,
			10.f);
	}
#endif
}

void UANS_CameraSpline::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{

	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (SplinePack == nullptr)
		return;

	if (UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		FAttachmentTransformRules AttachmentRule{ EAttachmentRule::SnapToTarget,false };
		if (ScratchPad->CameraComponent && ScratchPad->SpringArmComponent)
		{
			ScratchPad->CameraComponent->AttachToComponent(ScratchPad->SpringArmComponent, AttachmentRule);
		}
	}

}

UANS_ScratchPad* UANS_CameraSpline::CreateScratchPad(UObject* Outer)
{	
	return NewObject<UANS_CameraSplineScratchPad>(Outer);
}
