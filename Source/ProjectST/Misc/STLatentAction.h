// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/WeakObjectPtr.h"
#include "Components/SceneComponent.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "LatentActions.h"

UENUM(BlueprintType)
enum class EParabolicMoveToInputType :uint8
{
	Allow,
	Block,
	BlockAscending,
};

UENUM(BlueprintType)
enum class EParabolicMoveToResult :uint8
{
	OnLand,
	Fail
};

UENUM(BlueprintType)
enum class EMoveToInputType :uint8
{
	Allow,
	Block
};

/** Action that interpolates a component over time to a desired position */
class FSplineMoveToAction : public FPendingLatentAction
{
public:
	/** Time over which interpolation should happen */
	float TotalTime;
	/** Time so far elapsed for the interpolation */
	float TimeElapsed;
	/** If we are currently interpolating. If false, update will complete */
	bool bInterpolating;

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Component to interpolate */
	TWeakObjectPtr<ACharacter> Target;
	
	TWeakObjectPtr<UCharacterMovementComponent> MovementComp;

	FVector FocalPoint;

	TWeakObjectPtr<AActor> FocalActor;

	/** Should we ease in (ie start slowly) during interpolation */
	bool bEaseIn;
	/** Should we east out (ie end slowly) during interpolation */
	bool bEaseOut;

	bool bIsSplineWorldPos;

	bool bIsVelocityBase;

	FTransform LastTransform;

	FTransform InitialTransform;

	class USTSplinePack* SplinePack;

	EMoveToInputType InputType;

	bool bOnceFired;


	bool bShouldEnd;

	FSplineMoveToAction(float Duration, const FLatentActionInfo& LatentInfo, ACharacter* Character, bool bInEaseOut, bool bInEaseIn, USTSplinePack* SplinePack)
		: TotalTime(Duration)
		, TimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, Target(Character)
		, FocalPoint(FVector::ZeroVector)
		, bEaseIn(bInEaseIn)
		, bEaseOut(bInEaseOut)
		, bIsSplineWorldPos(false)
		, bIsVelocityBase(false)
		, LastTransform(FTransform::Identity)
		, SplinePack(SplinePack)
		, bShouldEnd(false)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

};

class UCharacterMovementComponent;

/** Action that interpolates a component over time to a desired position */
class FParabolicMoveToAction : public FPendingLatentAction
{
public:

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Component to interpolate */
	TWeakObjectPtr<ACharacter> Character;

	TWeakObjectPtr<UCharacterMovementComponent> MovementComp;
	FVector StartLocation;

	FVector TargetLocation;

	float LaunchSpeedScale;

	ESuggestProjVelocityTraceOption::Type CollisionType;

	EParabolicMoveToResult* Result;

	float CollisionRadius;

	bool bFavorHighArc;

	bool bOnceFired;

	bool bDebugPath;

	bool bAcceptClosestOnNoSolutions;

	float InitialBrakingDeceleration;

	EParabolicMoveToInputType InputType;

	float TimeElapsed;

	bool bShouldEnd;

	FParabolicMoveToAction(const FLatentActionInfo& LatentInfo, ACharacter* CharacterOwner)
		: ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, Character(CharacterOwner)
		, bOnceFired(false)
		, InitialBrakingDeceleration(0.f)
		, bShouldEnd(false)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;

};


/** Action that interpolates a component over time to a desired position */
class FInterpolateRotationOfComponentToAction : public FPendingLatentAction
{
public:
	TWeakObjectPtr<ACharacter> Character;

	bool bOnceFired;
	/** Time over which interpolation should happen */
	float TotalTime;
	/** Time so far elapsed for the interpolation */
	float TimeElapsed;
	/** If we are currently interpolating. If false, update will complete */
	bool bInterpolating;

	/** Function to execute on completion */
	FName ExecutionFunction;
	/** Link to fire on completion */
	int32 OutputLink;
	/** Object to call callback on upon completion */
	FWeakObjectPtr CallbackTarget;

	/** Component to interpolate */
	TWeakObjectPtr<USceneComponent> TargetComponent;

	/** If we should modify rotation */
	bool bInterpRotation;
	/** Rotation to interpolate from */
	FRotator InitialRotation;
	/** Rotation to interpolate to */
	FRotator TargetRotation;

	/** Should we ease in (ie start slowly) during interpolation */
	bool bEaseIn;
	/** Should we east out (ie end slowly) during interpolation */
	bool bEaseOut;

	/** Force use of shortest Path for rotation **/
	bool bForceShortestRotationPath;

	bool bShouldEnd;

	FInterpolateRotationOfComponentToAction(float Duration, const FLatentActionInfo& LatentInfo, USceneComponent* Component, bool bInEaseOut, bool bInEaseIn, bool bInForceShortestRotationPath)
		: bOnceFired(false)
		, TotalTime(Duration)
		, TimeElapsed(0.f)
		, bInterpolating(true)
		, ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, TargetComponent(Component)
		, bInterpRotation(true)
		, InitialRotation(FRotator::ZeroRotator)
		, TargetRotation(FRotator::ZeroRotator)
		, bEaseIn(bInEaseIn)
		, bEaseOut(bInEaseOut)
		, bForceShortestRotationPath(bInForceShortestRotationPath)
		, bShouldEnd(false)
	{
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;
};
