// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STManagedStates.h"
#include "STManagedState_LockOn.generated.h"


UCLASS()
class USTManagedState_LockOn : public USTManagedState
{
	GENERATED_BODY()

public:
	USTManagedState_LockOn();

	virtual void OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;
	virtual void OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)override;
	virtual void OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;

	AActor* FindLockOnTarget(AActor* StateOwner);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<TEnumAsByte<EObjectTypeQuery>> LockOnTargetTypes;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float LockOnCaptureRadius = 800.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float LockOnCaptureMaxAngle = 75.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InterpSpeed = 10.f;


	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLockOnTargetSet, AActor*, Target);
	UPROPERTY(BlueprintAssignable)
	FOnLockOnTargetSet OnLockOnTargetSet;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLockOnTargetRemoved);
	UPROPERTY(BlueprintAssignable)
	FOnLockOnTargetRemoved OnLockOnTargetRemoved;

private:

	UPROPERTY()
	ACharacter* OwnerAsCharacter;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AActor* LockOnTarget;

	FRotator TargetRotation;

};


