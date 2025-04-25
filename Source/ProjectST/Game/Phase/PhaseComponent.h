// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhaseComponent.generated.h"

USTRUCT(BlueprintType)
struct FPhaseInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 PauseDuration;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 ShrinkDuration;
};

USTRUCT(BlueprintType)
struct FPhaseContext
{
	GENERATED_BODY()

	void IncrementPhase()
	{
		++CurrentPhase;
		bIsShrinking = false;
		ElapsedTimeOnPhase = 0;
	}

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 CurrentGameTime = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 CurrentPhase = 0;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	int32 ElapsedTimeOnPhase = 0;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsShrinking = false;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bIsPhaseOver = false;

};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseShrink,const FPhaseContext&,CurrentPhaseContext,const FPhaseInfo&, PhaseInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseTick, const FPhaseContext&, CurrentPhaseContext, const FPhaseInfo&, PhaseInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPhaseIncremented, const FPhaseContext&, CurrentPhaseContext, const FPhaseInfo&, PhaseInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPhaseEnded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API UPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

public:	
	
	UFUNCTION()
	void ProcessPhaseTick();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_CurrentPhaseContext(const FPhaseContext& OldData);

private:

	UPROPERTY(BlueprintAssignable)
	FOnPhaseTick OnPhaseTick;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseShrink OnPhaseShrink;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseIncremented OnPhaseIncremented;

	UPROPERTY(BlueprintAssignable)
	FOnPhaseEnded OnPhaseEnded;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 GameTime = 1800;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<FPhaseInfo> PhaseInfo;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentPhaseContext,VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FPhaseContext CurrentPhaseContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	float PhaseTickFrequency = 1.f;

	FTimerHandle PhaseTickTimer;
};
