// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterMovementPredictionData.h"
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float SprintSpeed = 680.f;
};


UCLASS()
class PROJECTST_API USTCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USTCharacterMovementComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;

	virtual void ServerMove_PerformMovement(const FCharacterNetworkMoveData& MoveData);

	UFUNCTION(Blueprintpure)
	const FDefaultMovementStats& GetDefaultMovementStat() const { return DefaultStat; }

	UFUNCTION(BlueprintCallable)
	FVector GetLastInputVector_Rep();

	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)override;


private:

//////// Predicted Data
	UPROPERTY(Replicated)
	FVector LastInputVector;

/////////////////////


	FSTCharacterNetworkMoveDataContainer STNetworkMoveDataContainer;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	FDefaultMovementStats DefaultStat;

};
