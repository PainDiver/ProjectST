// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STCharacterBase.h"
#include "STEnemyCharacter.generated.h"

/**
 * 
 */
class USTInventoryComponent;
class USTStateTreeComponent;
class USTAIPerceptionComponent;
class ASTAIController;
class USTDataAsset_AI;

UCLASS()
class PROJECTST_API ASTEnemyCharacter : public ASTCharacterBase
{
	GENERATED_BODY()
	
public:
	ASTEnemyCharacter(const FObjectInitializer& OI);
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	ASTAIController* GetSTAIController()const;

	UFUNCTION(BlueprintPure)
	USTDataAsset_AI* GetAIData()const { return AIDataInfo; }

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	USTDataAsset_AI* AIDataInfo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTInventoryComponent> InventoryComponent;

};
