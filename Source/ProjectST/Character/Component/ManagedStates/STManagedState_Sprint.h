// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STManagedStates.h"
#include "STManagedState_Sprint.generated.h"


class USTCharacterMovementComponent;

UCLASS()
class USTManagedState_Sprint : public USTManagedState
{
	GENERATED_BODY()

public:
	USTManagedState_Sprint();

	virtual bool CanAddState_Implementation(AActor* StateOwner);
	virtual void OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;
	virtual void OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime)override;
	virtual void OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent)override;

private:

	UPROPERTY()
	USTCharacterMovementComponent* MovementComp;
};


