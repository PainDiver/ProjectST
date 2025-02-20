// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "STDataAsset_Input.generated.h"

/**
 * 
 */

UENUM(Blueprintable,BlueprintType)
enum class EInputType : uint8
{
	NONE,
	IT_MOVE,
	IT_LOOK,
	IT_JUMP,
	IT_WEAK_ATTACK,
	IT_GUARD,
	IT_SWAY
};

UENUM(Blueprintable, BlueprintType)
enum class EActionFunctionType : uint8
{
	//DisplayName을 이용해서 실제 함수를 바인딩해야함

	Move			UMETA(DisplayName = "Move"),
	Look			UMETA(DisplayName = "Look"),
	Jump			UMETA(DisplayName = "Jump"),
	StopJumping		UMETA(DisplayName = "StopJumping"),
	WeakAttack		UMETA(DisplayName = "ProcessWeakAttack"),
	Guard			UMETA(DisplayName = "ProcessGuard"),
	Sway			UMETA(DisplayName = "ProcessSway")
};


USTRUCT(Blueprintable, BlueprintType)
struct PROJECTST_API FInputDataBinder
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETriggerEvent TriggerType;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UInputAction* InputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EActionFunctionType Function;
};


UCLASS()
class PROJECTST_API USTDataAsset_Input : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TArray<FInputDataBinder> TagInputs;
};
