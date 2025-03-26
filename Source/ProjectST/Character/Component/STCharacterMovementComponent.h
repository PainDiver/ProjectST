// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "STCharacterMovementComponent.generated.h"

USTRUCT(BlueprintType)
struct FDefaultMovementStats
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float RotationRate = 270.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float NormalSpeed = 500.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GuardingSpeed = 175.f;
};


UCLASS()
class PROJECTST_API USTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USTCharacterMovementComponent();

	virtual void BeginPlay() override;

	UFUNCTION(Blueprintpure)
	const FDefaultMovementStats& GetDefaultMovementStat() const { return DefaultStat; }


	virtual void ProcessLanded(const FHitResult& Hit, float remainingTime, int32 Iterations)override;
	virtual bool DoJump(bool bReplayingMoves, float DeltaTime)override;


private:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FDefaultMovementStats DefaultStat;

};
