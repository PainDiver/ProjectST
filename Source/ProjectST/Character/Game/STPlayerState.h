// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GAS/STAbilitySystemComponent.h"
#include "Character/Component/STInventoryComponent.h"
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

	UFUNCTION(BlueprintCallable)
	USTInventoryComponent* GetInventoryComponent()const { return InventoryComponent; }
private:

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTInventoryComponent> InventoryComponent;
};
