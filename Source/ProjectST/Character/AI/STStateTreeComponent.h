// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StateTreeComponent.h"
#include "STStateTreeComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure)
	bool HasLinkedStateTree(const FGameplayTag& Tag);
	
};
