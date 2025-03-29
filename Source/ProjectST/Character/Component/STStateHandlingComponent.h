// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "STStateHandlingComponent.generated.h"

class USTManagedState;

UCLASS(BlueprintType,Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTStateHandlingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTStateHandlingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void RestoreLastStateEffect();

	void AddToStateOnRunning(const FGameplayTag& NewStack);

	void RemoveStateOnRunning(const FGameplayTag& NewStack);

	bool CanAddState(const FGameplayTag& Tag);

private:

	UPROPERTY()
	AActor* Owner;

	UPROPERTY()
	TArray<FGameplayTag> StateOnRunning;

	UPROPERTY(instanced,BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TArray<USTManagedState*> States;

	UPROPERTY()
	FGameplayTagContainer PreviousContainer;
};
