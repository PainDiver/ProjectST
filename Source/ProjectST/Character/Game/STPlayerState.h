// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GAS/STAbilitySystemComponent.h"

#include "STPlayerState.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTST_API ASTPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ASTPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;


};
