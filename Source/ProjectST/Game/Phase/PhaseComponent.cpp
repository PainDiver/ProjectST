// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Phase/PhaseComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
// Sets default values for this component's properties
UPhaseComponent::UPhaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UPhaseComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentPhaseContext.CurrentGameTime = GameTime;

	if(GetOwner()->HasAuthority())
		GetWorld()->GetTimerManager().SetTimer(PhaseTickTimer,FTimerDelegate::CreateUObject(this,&ThisClass::ProcessPhaseTick),PhaseTickFrequency,true);
}

void UPhaseComponent::BeginDestroy()
{
	if(GetWorld())
		GetWorld()->GetTimerManager().ClearTimer(PhaseTickTimer);

	Super::BeginDestroy();
}


void UPhaseComponent::ProcessPhaseTick()
{
	--CurrentPhaseContext.CurrentGameTime;
	++CurrentPhaseContext.ElapsedTimeOnPhase;

	if (CurrentPhaseContext.bIsPhaseOver || !PhaseInfo.IsValidIndex(CurrentPhaseContext.CurrentPhase))
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(UPhaseComponent, CurrentPhaseContext, this);
		return;
	}

	const FPhaseInfo& CurrentPhaseInfo = PhaseInfo[CurrentPhaseContext.CurrentPhase];
	OnPhaseTick.Broadcast(CurrentPhaseContext, CurrentPhaseInfo);
	if (CurrentPhaseContext.bIsShrinking && CurrentPhaseContext.ElapsedTimeOnPhase >= CurrentPhaseInfo.ShrinkDuration)
	{
		if (!PhaseInfo.IsValidIndex(CurrentPhaseContext.CurrentPhase + 1))
		{
			CurrentPhaseContext.bIsPhaseOver = true;
			OnPhaseEnded.Broadcast();
		}
		else
		{
			CurrentPhaseContext.IncrementPhase();
			const FPhaseInfo& NewPhaseInfo = PhaseInfo[CurrentPhaseContext.CurrentPhase];
			OnPhaseIncremented.Broadcast(CurrentPhaseContext, NewPhaseInfo);
		}	
	}
	else if(CurrentPhaseContext.ElapsedTimeOnPhase >= CurrentPhaseInfo.PauseDuration)
	{
		CurrentPhaseContext.bIsShrinking = true;		
		OnPhaseShrink.Broadcast(CurrentPhaseContext, CurrentPhaseInfo);
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(UPhaseComponent, CurrentPhaseContext, this);

}

void UPhaseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(UPhaseComponent, CurrentPhaseContext,Params );
}

void UPhaseComponent::OnRep_CurrentPhaseContext(const FPhaseContext& OldData)
{
	if (OldData.CurrentGameTime != CurrentPhaseContext.CurrentGameTime)
	{
		OnPhaseTick.Broadcast(CurrentPhaseContext,PhaseInfo[CurrentPhaseContext.CurrentPhase]);
	}

	if (OldData.CurrentPhase != CurrentPhaseContext.CurrentPhase)
	{
		OnPhaseIncremented.Broadcast(CurrentPhaseContext, PhaseInfo[CurrentPhaseContext.CurrentPhase]);	
	}
	else if (OldData.bIsShrinking != CurrentPhaseContext.bIsShrinking)
	{
		OnPhaseShrink.Broadcast(CurrentPhaseContext, PhaseInfo[CurrentPhaseContext.CurrentPhase]);
	}
	else if (OldData.bIsPhaseOver != CurrentPhaseContext.bIsPhaseOver)
	{
		OnPhaseEnded.Broadcast();
	}
}

