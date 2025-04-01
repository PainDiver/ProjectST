// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemComponent.h"
#include "Params.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct FHitParam : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName Desc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer TargetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag HitType;

	// Push,Pull,PullToHitter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag KnockbackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float KnockbackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag HitCue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 ExecutionTableRow;
};

USTRUCT(BlueprintType, Blueprintable)
struct FExecutionInfo : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName Desc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ExecutionHitMontage;
};

USTRUCT(BlueprintType,Blueprintable)
struct FHitInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	AActor* Attacker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* Defender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 HitInfoIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeStamp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayEventData DataIfSet;
};