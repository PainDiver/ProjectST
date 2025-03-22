// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Params.generated.h"

UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class PROJECTST_API UHitParam : public UObject
{
	GENERATED_BODY()
public:

	bool IsValidHitParam();

private:

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(AllowPrivateAccess = "true"))
	FGameplayTagContainer TargetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float BaseDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag HitType;

	// Push,Pull,PullToHitter
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag KnockbackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float KnockbackTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float KnockbackDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UParticleSystem* HitParticle;

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
	UHitParam* HitParam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TimeStamp;
};