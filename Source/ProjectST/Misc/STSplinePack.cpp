// Fill out your copyright notice in the Description page of Project Settings.


#include "STSplinePack.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "UObject/SavePackage.h"


ASTSplinePackGenerator::ASTSplinePackGenerator()
:AActor()
{
	EditingSplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("EditingSplineComponent"));
}
#if WITH_EDITOR
void ASTSplinePackGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SplinePackToModify == nullptr)
	{
		return;
	}

	USTSplinePack* SplinePack = SplinePackToModify->GetDefaultObject<USTSplinePack>();
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
#endif

void ASTSplinePackGenerator::PostLoad()
{
	Super::PostLoad();

	if (SplinePackToModify == nullptr)
	{
		return;
	}

	USTSplinePack* SplinePack = SplinePackToModify->GetDefaultObject<USTSplinePack>();
	if (SplinePack == nullptr)
	{
		return;
	}

	EditingSplineComponent->SplineCurves = SplinePack->SplineCurves;	
	EditMode = ESplinePackEditMode::None;
}


FVector USTSplinePack::GetLocationAtSplineInputKey(const FTransform& CameraTransform, float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const
{
	FVector Location = SplineCurves.Position.Eval(InKey, FVector::ZeroVector);

	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		Location = CameraTransform.TransformPosition(Location);
	}

	return Location;
}

FQuat USTSplinePack::GetQuaternionAtSplineInputKey(const FTransform& CameraTransform,float InKey, ESplineCoordinateSpace::Type CoordinateSpace) const
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

FVector USTSplinePack::GetScaleAtSplineInputKey(float InKey) const
{
	const FVector Scale = SplineCurves.Scale.Eval(InKey, FVector(1.0f));
	return Scale;
}

FTransform USTSplinePack::GetTransformAtSplineInputKey(const FTransform& OwnerTransform,const FVector& FocalStart ,const FVector& FocalPoint,float InKey, ESplineCoordinateSpace::Type CoordinateSpace, bool bUseOnlyYaw, bool bUseScale) const
{
	const FVector Location(GetLocationAtSplineInputKey(OwnerTransform, InKey, ESplineCoordinateSpace::Local));
	
	FQuat Rotation;
	if (!bAlwaysLookAtOwner)
	{	
		Rotation = GetQuaternionAtSplineInputKey(OwnerTransform, InKey, ESplineCoordinateSpace::Local);
	}

	const FVector Scale = bUseScale ? GetScaleAtSplineInputKey(InKey) : FVector(1.0f);

	FTransform Transform(Rotation, Location, Scale);

	if (CoordinateSpace == ESplineCoordinateSpace::World)
	{
		Transform = Transform * OwnerTransform;
	}

	if (bAlwaysLookAtOwner)
	{

		FRotator Rot = (UKismetMathLibrary::FindLookAtRotation(FocalStart, FocalPoint));
		Rotation = Rot.Quaternion();
		if (bUseOnlyYaw)
		{
			Rot.Pitch = 0.f;
			Rot.Roll = 0.f;
		}
		Transform.SetRotation(Rotation);
	}

	return Transform;
}

