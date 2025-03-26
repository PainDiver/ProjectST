// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "STManagedStates.generated.h"

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class USTManagedState : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void OnStateAdded(AActor* StateOwner);
	virtual void OnStateAdded_Implementation(AActor* StateOwner) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnTick(AActor* StateOwner,float DeltaTime);
	virtual void OnTick_Implementation(AActor* StateOwner,float DeltaTime);

	UFUNCTION(BlueprintNativeEvent)
	void OnStateRemoved(AActor* StateOwner);
	virtual void OnStateRemoved_Implementation(AActor* StateOwner)
	{
		bIsRemoved = true;
	};

	bool IsRemoved() const { return bIsRemoved; }

	bool IsMatchingState(const FGameplayTag Tag);

	FGameplayTag& GetTag() { return StateTag; }

private:

	bool bIsRemoved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag StateTag;
};

class USTCharacterMovementComponent;

UCLASS()
class USTManagedState_Guard : public USTManagedState
{
	GENERATED_BODY()

public:
	USTManagedState_Guard();

	virtual void OnStateAdded_Implementation(AActor* StateOwner)override;
	virtual void OnTick_Implementation(AActor* StateOwner, float DeltaTime)override;
	virtual void OnStateRemoved_Implementation(AActor* StateOwner)override;

private:

	FGameplayTagContainer TagNotAllowed;

	UPROPERTY()
	USTCharacterMovementComponent* MovementComp;
};