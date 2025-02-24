// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerState.h"

ASTPlayerState::ASTPlayerState()
	:Super()
{
	AbilitySystemComponent = CreateDefaultSubobject<USTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	InventoryComponent = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComponent"));
	SetReplicates(true);
	NetUpdateFrequency = 100.f;
}

UAbilitySystemComponent* ASTPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

