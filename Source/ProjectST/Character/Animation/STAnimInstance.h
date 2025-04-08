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
class ASTCharacterBase;
class USTCharacterMovementComponent;

UCLASS()
class PROJECTST_API USTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay()override;

	void SetInitialVars();

	virtual void NativeUpdateAnimation(float DeltaSeconds)override;

	UFUNCTION(BlueprintNativeEvent)
	bool ShouldUpdateTurnValue();
	bool ShouldUpdateTurnValue_Implementation();

	void UpdateTurnValue(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void ResetTurn();

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

	UPROPERTY()
	TMap<int32, UANS_ScratchPad*> CollapsedCachedScratchPad;

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ASTCharacterBase* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USkeletalMeshComponent* OwingComponent;;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USTCharacterMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsDedicateServer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsMoving;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsGuarding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsSprinting;

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
	FVector Acceleration_World;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector Acceleration_Local_Ratio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector TiltRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector LastWorldLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DisplacementSinceLastUpdate;

	UPROPERTY()
	bool bIsTurnLocked;

	UPROPERTY()
	bool bIsTurning;

	UPROPERTY()
	FVector LockedTurnValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TurnValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsLeftFootAhead;


	//Settings
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	float Acceleration_Local_Ratio_InterpSpeed = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	float Turn_InterpSpeed = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Settings)
	float TurnAngle = 90.f;


};
