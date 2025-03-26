// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimNotify/ANS_Base.h"
#include "STEnum_Anim.h"
#include "Misc/STEnum.h"
#include "STAnimInstance.generated.h"


class UBoxComponent;
class USphereComponent;
class UCapsuleComponent;

USTRUCT(BlueprintType)
struct FVectorRatio
{
	GENERATED_BODY()
public:
	float F;
	float B;
	float L;
	float R;
};

UCLASS()
class PROJECTST_API USTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay()override;

	virtual void NativeUpdateAnimation(float DeltaSeconds)override;


	void InitializeCachedShapes();

	virtual bool ShouldTriggerAnimNotifyState(const UAnimNotifyState* AnimNotifyState) const override;

	UFUNCTION()
	void OnMontageEnd(UAnimMontage* Montage, bool bInterrupted);

	void CacheScratchPad(int32 Key,UANS_ScratchPad* ScratchPad);
	void RemoveScratchPad(int32 Key);
	UANS_ScratchPad* GetCachedScratchPad(int32 Key);
	UShapeComponent* GetCachedShape(ETargetQueryType ShapeType);

private:
	UPROPERTY()
	TArray<UBoxComponent*> BoxTargetCollision;
	UPROPERTY()
	TArray<USphereComponent*> SphereTargetCollision;
	UPROPERTY()
	TArray<UCapsuleComponent*> CapsuleTargetCollision;

	UPROPERTY()
	TMap<int32,UANS_ScratchPad*> CachedScratchPad;



public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGuarding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Speed_Ratio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Velocity_Local;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FVector Velocity_World;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D Velocity_Ratio;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Acceleration;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVectorRatio AccelerationRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction;
};
