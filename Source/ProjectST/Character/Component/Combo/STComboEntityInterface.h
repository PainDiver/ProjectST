// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Game/DataAsset/STDataAsset_Input.h"
#include "STComboEntityInterface.generated.h"



USTRUCT(Blueprintable)
struct FInputDetail
{
	GENERATED_BODY()

public:
	FInputDetail() = default;

	FInputDetail(EInputType Type, ETriggerEvent TriggerType)
	{
		InputType = Type;
		InputTriggerType = TriggerType;
	}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EInputType InputType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerEvent InputTriggerType;

	bool operator==(const FInputDetail& Other) const
	{
		return Other.InputTriggerType == InputTriggerType &&
			Other.InputType == InputType;
	}

	friend uint32 GetTypeHash(const FInputDetail& Input)
	{
		return HashCombine(GetTypeHash(Input.InputType), static_cast<uint32>(Input.InputTriggerType));
	}

};


USTRUCT(Blueprintable)
struct FComboWindowContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FInputDetail, FGameplayTag> InputToGA;

	bool bIsReset = false;

	void Set(const FComboWindowContext& NewWindow)
	{
		if (bIsReset == true)
		{
			InputToGA = NewWindow.InputToGA;
		}
		else
		{
			InputToGA.Append(NewWindow.InputToGA);
		}
		bIsReset = false;
	}

	void Reset()
	{
		InputToGA.Empty();
		bIsReset = true;
	}
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTComboEntityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTST_API ISTComboEntityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void SetComboContext(const FComboWindowContext& NewWindow) = 0;
	
	virtual void ClearComboContext() = 0;
	
	virtual void FlushCombo() = 0;

};
