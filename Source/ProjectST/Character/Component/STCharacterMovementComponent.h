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
};


UCLASS()
class PROJECTST_API USTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	
	virtual void BeginPlay() override;

	UFUNCTION(Blueprintpure)
	FDefaultMovementStats GetDefaultMovementStat() const { return DefaultStat; }

private:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FDefaultMovementStats DefaultStat;

};
