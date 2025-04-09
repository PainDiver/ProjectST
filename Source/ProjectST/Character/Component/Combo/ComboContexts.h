// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/DataAsset/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "ComboContexts.generated.h"

class UAbilitySystemComponent;
class USTComboManagingComponent;

UCLASS(BlueprintType)
class UComboInputData : public UObject
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	ESTInputType InputType;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FInputActionInstance InputInstance;
};

UCLASS()
class UComboContext : public UObject
{
	GENERATED_BODY()
public:

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack);

	// 내부 팩토리 이용해서 런타임 Enum -> Type생성
	static UComboContext* CreateContext(UObject* Outer, EComboContextState State);

	EComboContextState ComboContextState;
};

UCLASS()
class UComboContext_Default : public UComboContext
{
	GENERATED_BODY()
public:

	UComboContext_Default() :
		UComboContext()
	{
		ComboContextState = EComboContextState::DEFAULT;
	}

	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC,USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)override;

};

UCLASS()
class UComboContext_Jumping : public UComboContext
{
	GENERATED_BODY()
public:
	UComboContext_Jumping() :
		UComboContext()
	{
		ComboContextState = EComboContextState::JUMPING;
	}
	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)override;
};


UCLASS()
class UComboContext_OnHit : public UComboContext
{
	GENERATED_BODY()
public:
	UComboContext_OnHit() :
		UComboContext()
	{
		ComboContextState = EComboContextState::ON_HIT;
	}
	virtual void ProcessCombo(UAbilitySystemComponent* OwnerASC, USTComboManagingComponent* ComboManaingComp, ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)override;
};

