// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STManagedStates.h"
#include "STManagedState_Guard.generated.h"

class USTCharacterMovementComponent;

UCLASS()
class USTManagedState_Guard : public USTManagedState
{
	GENERATED_BODY()

public:
	USTManagedState_Guard();

	virtual void OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;
	virtual void OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)override;
	virtual void OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;

private:

	UPROPERTY()
	USTCharacterMovementComponent* MovementComp;
};

