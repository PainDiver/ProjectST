// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STCharacterBase.h"
#include "STEnemyCharacter.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTST_API ASTEnemyCharacter : public ASTCharacterBase
{
	GENERATED_BODY()
	
public:
	ASTEnemyCharacter();
	
	virtual void BeginPlay() override;


};
