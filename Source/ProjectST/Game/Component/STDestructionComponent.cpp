// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Component/STDestructionComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

// Sets default values for this component's properties
USTDestructionComponent::USTDestructionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USTDestructionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void USTDestructionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = ELifetimeCondition::COND_None;

	DOREPLIFETIME_WITH_PARAMS_FAST(USTDestructionComponent,BreakHP,Params)
}

void USTDestructionComponent::DamageObject(int32 BreakDamage)
{
	if (BreakHP <= 0)
		return;

	OnDamageDelegate.Broadcast(BreakDamage);

	BreakHP -= BreakDamage;
	if (BreakHP <= 0)
	{
		Destruct();
	}
	MARK_PROPERTY_DIRTY_FROM_NAME(USTDestructionComponent, BreakHP, this);
}

void USTDestructionComponent::Destruct()
{
	OnDestructionDelegate.Broadcast();
}

void USTDestructionComponent::OnRep_BreakHP(int32 OldBreakHP)
{
	if(OldBreakHP > BreakHP)
		OnDamageDelegate.Broadcast(OldBreakHP - BreakHP);

	if (BreakHP <= 0)
	{
		Destruct();
	}
}
