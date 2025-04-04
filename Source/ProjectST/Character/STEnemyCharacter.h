// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STCharacterBase.h"
#include "STEnemyCharacter.generated.h"

/**
 * 
 */
class USTInventoryComponent;

UCLASS()
class PROJECTST_API ASTEnemyCharacter : public ASTCharacterBase
{
	GENERATED_BODY()
	
public:
	ASTEnemyCharacter(const FObjectInitializer& OI);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;

	virtual void BeginPlay() override;

private:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTInventoryComponent> InventoryComponent;
};
