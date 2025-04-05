// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_CameraSpline.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/SavePackage.h"


ASplinePackGenerator::ASplinePackGenerator()
:AActor()
{
	EditingSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("EditingSplineComponent"));
}

void ASplinePackGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SplinePackToModify == nullptr)
	{
		return;
	}

	UCameraSplinePack* SplinePack = SplinePackToModify->GetDefaultObject<UCameraSplinePack>();
	if (SplinePack == nullptr)
	{
		return;
	}
	switch (EditMode)
	{
		case ESplinePackEditMode::Save:
		{
			SplinePack->SplineCurves = EditingSplineComponent->SplineCurves;
			break;
		}
		case ESplinePackEditMode::Load:
		{
			EditingSplineComponent->SplineCurves = SplinePack->SplineCurves;
			break;
		}
		case ESplinePackEditMode::Clear:
		{
			EditingSplineComponent->ResetToDefault();
			break;
		}
	}

	UPackage* Package = SplinePack->GetOutermost();
	FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;
	bool bSuccess = UPackage::SavePackage(Package, SplinePack, *PackageFileName, SaveArgs);

	EditMode = ESplinePackEditMode::None;
}

void ASplinePackGenerator::PostLoad()
{
	Super::PostLoad();

	if (SplinePackToModify == nullptr)
	{
		return;
	}

	UCameraSplinePack* SplinePack = SplinePackToModify->GetDefaultObject<UCameraSplinePack>();
	if (SplinePack == nullptr)
	{
		return;
	}

	EditingSplineComponent->SplineCurves = SplinePack->SplineCurves;	
	EditMode = ESplinePackEditMode::None;
}


FVector UCameraSplinePack::GetLocationAtSplineInputKey(const FTransform& CameraTransform, float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const
{
	FVector Location = SplineCurves.Position.Eval(InKey, FVector::ZeroVector);

	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		Location = CameraTransform.TransformPosition(Location);
	}

	return Location;
}

FQuat UCameraSplinePack::GetQuaternionAtSplineInputKey(const FTransform& CameraTransform,float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const
{
	FQuat Quat = SplineCurves.Rotation.Eval(InKey, FQuat::Identity);
	Quat.Normalize();

	const FVector Direction = SplineCurves.Position.EvalDerivative(InKey, FVector::ZeroVector).GetSafeNormal();
	const FVector UpVector = Quat.RotateVector(FVector::UpVector);

	FQuat Rot = (FRotationMatrix::MakeFromXZ(Direction, UpVector)).ToQuat();

	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		Rot = CameraTransform.GetRotation() * Rot;
	}

	return Rot;
}

FVector UCameraSplinePack::GetScaleAtSplineInputKey(float InKey) const
{
	const FVector Scale = SplineCurves.Scale.Eval(InKey, FVector(1.0f));
	return Scale;
}

FTransform UCameraSplinePack::GetTransformAtSplineInputKey(AActor* Owner, const FVector& CameraLocation,float InKey, ESplineCoordinateSpace::Type CoordinateSpace, bool bUseScale) const
{
	if (Owner == nullptr)
		return FTransform();

	const FVector Location(GetLocationAtSplineInputKey(Owner->GetTransform(), InKey, ESplineCoordinateSpace::Local));
	
	FQuat Rotation;
	if (!bAlwaysLookAtOwner)
	{	
		Rotation = GetQuaternionAtSplineInputKey(Owner->GetTransform(), InKey, ESplineCoordinateSpace::Local);
	}

	const FVector Scale = bUseScale ? GetScaleAtSplineInputKey(InKey) : FVector(1.0f);

	FTransform Transform(Rotation, Location, Scale);

	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		Transform = Transform * Owner->GetTransform();
	}

	if (bAlwaysLookAtOwner)
	{
		Rotation = (UKismetMathLibrary::FindLookAtRotation(CameraLocation, Owner->GetActorLocation())).Quaternion();
		Transform.SetRotation(Rotation);
	}

	return Transform;
}



void UANS_CameraSpline::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
	if (ScratchPad == nullptr || SplinePack == nullptr)
	{
		return;
	}

	ScratchPad->Owner = MeshComp->GetOwner();	
	ScratchPad->CameraComponent = ScratchPad->Owner->GetComponentByClass<UCameraComponent>();	 	
	ScratchPad->SpringArmComponent = ScratchPad->Owner->GetComponentByClass<USpringArmComponent>();
	ScratchPad->SplinePack = Cast<UCameraSplinePack>(SplinePack->GetDefaultObject());
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

	if (SplinePack == nullptr)
		return;

	UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
	if (ScratchPad == nullptr || ScratchPad->Owner == nullptr|| ScratchPad->CameraComponent == nullptr)
	{
		return;
	}	

	ScratchPad->ElapsedTime += FrameDeltaTime;

	float SplineDuration = ScratchPad->SplinePack->SplineCurves.Position.Points.Last().InVal;
	float NomalizingRatio = SplineDuration / EventReference.GetNotify()->Duration;

	float Progress = ScratchPad->ElapsedTime * NomalizingRatio;

	FTransform Transform = ScratchPad->CameraComponent->GetComponentTransform();	
	FTransform NewTransform = ScratchPad->SplinePack->GetTransformAtSplineInputKey(ScratchPad->Owner, ScratchPad->CameraComponent->GetComponentLocation(), Progress, ESplineCoordinateSpace::World, false);

	FVector NewLoc = FMath::VInterpTo(Transform.GetLocation(),NewTransform.GetLocation(),FrameDeltaTime , ScratchPad->SplinePack->InterpSpeed);
	FRotator NewRot = FMath::RInterpTo(Transform.GetRotation().Rotator(), NewTransform.GetRotation().Rotator(), FrameDeltaTime, ScratchPad->SplinePack->InterpSpeed);
	FTransform InterpedTransform{ NewRot,NewLoc,FVector::OneVector };
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
	UANS_CameraSplineScratchPad* ScratchPad = Cast<UANS_CameraSplineScratchPad>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
	if (ScratchPad == nullptr || ScratchPad->Owner == nullptr || ScratchPad->CameraComponent == nullptr)
	{
		return;
	}

	FAttachmentTransformRules AttachmentRule{EAttachmentRule::SnapToTarget,false};
	ScratchPad->CameraComponent->AttachToComponent(ScratchPad->SpringArmComponent, AttachmentRule);
}

UANS_ScratchPad* UANS_CameraSpline::CreateScratchPad(UObject* Outer)
{	
	return NewObject<UANS_CameraSplineScratchPad>(Outer);
}
