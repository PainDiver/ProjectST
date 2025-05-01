// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "STAIController.generated.h"

class USTDataAsset_AI;
class USTAIPerceptionComponent;


UENUM(BlueprintType)
enum class EAIState: uint8
{
	Patrol,
	Trace,
	Combat,
	Flee,
};

UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	ASTAIController();

	virtual void BeginPlay()override;

	UFUNCTION(BlueprintCallable)
	void InitializeAILogic(USTDataAsset_AI* AIData);

	UFUNCTION(BlueprintPure)
	bool HasDynamicBT(const FGameplayTag& Tag);

private:

	UPROPERTY()
	USTDataAsset_AI* AssignedAIData;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTAIPerceptionComponent> AIPerceptionComponent;
};
