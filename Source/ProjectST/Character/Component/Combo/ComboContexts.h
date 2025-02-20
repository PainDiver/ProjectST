// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/DataAsset/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "ComboContexts.generated.h"

class UAbilitySystemComponent;
class USTComboManagingComponent;




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

