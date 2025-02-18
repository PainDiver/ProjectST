// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Control/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "ComboContexts.generated.h"

class UAbilitySystemComponent;
class USTComboManagingComponent;



// 컴파일 타임 Enum->Type 매핑
template<EComboContextState>
struct CombatStateClass
{};

template<>
struct CombatStateClass<EComboContextState::DEFAULT>
{
public:
	using Type = UComboContext_Default;
};

template<>
struct CombatStateClass<EComboContextState::JUMPING>
{
	using Type = UComboContext_Jumping;
};

template<>
struct CombatStateClass<EComboContextState::ON_HIT>
{
	using Type = UComboContext_OnHit;
};


UCLASS()
class UComboContext : public UObject
{
	GENERATED_BODY()
public:

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance) {};

	// 내부 팩토리 이용해서 런타임 Enum -> Type생성
	static UComboContext* CreateContext(UObject* Outer, EComboContextState State);

};

UCLASS()
class UComboContext_Default : public UComboContext
{
	GENERATED_BODY()
public:

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC,USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)override;
};

UCLASS()
class UComboContext_Jumping : public UComboContext
{
	GENERATED_BODY()
public:

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)override;
};


UCLASS()
class UComboContext_OnHit : public UComboContext
{
	GENERATED_BODY()
public:

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, EInputType InputType, const FInputActionInstance& InputInstance)override;
};

