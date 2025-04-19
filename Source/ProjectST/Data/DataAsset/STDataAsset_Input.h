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
enum class ESTInputType : uint8
{
	NONE,
// STANDING
	IT_MOVE,
	IT_LOOK,
	IT_JUMP,
	IT_WEAK_ATTACK,
	IT_GUARD,
	IT_SWAY,
	IT_SPRINT,
	IT_SkillQ,
	IT_SkillE,
	IT_LOCK_ON,
	IT_SkillR,
	IT_INTERACTION,
	IT_SELECTINTERACTION
};

UENUM(Blueprintable, BlueprintType)
enum class EActionFunctionType : uint8
{
	//DisplayName을 이용해서 실제 함수를 바인딩해야함

	Move			UMETA(DisplayName = "ProcessMove"),
	Look			UMETA(DisplayName = "ProcessLook"),
	Jump			UMETA(DisplayName = "ProcessJump"),
	StopJumping		UMETA(DisplayName = "StopJumping"),
	WeakAttack		UMETA(DisplayName = "ProcessWeakAttack"),
	Guard			UMETA(DisplayName = "ProcessGuard"),
	Sway			UMETA(DisplayName = "ProcessSway"),
	Sprint			UMETA(DisplayName = "ProcessSprint"),
	SkillQ			UMETA(DisplayName = "ProcessSkillQ"),
	SkillE			UMETA(DisplayName = "ProcessSkillE"),
	LockOn			UMETA(DisplayName = "ProcessLockOn"),
	SkillR			UMETA(DisplayName = "ProcessSkillR"),
	Interaction		UMETA(DisplayName = "ProcessInteraction"),
	SelectInteraction UMETA(DisplayName = "ProcessSelectInteraction"),
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
