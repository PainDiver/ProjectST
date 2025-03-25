// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GAS/STAbilitySystemComponent.h"
#include "Data/DataAsset/STDataAsset_Input.h"
#include "Character/Component/Combo/STComboEntityInterface.h"
#include "Game/Item/STItemContainerInterface.h"
#include "Character/STStateInterface.h"
#include "STCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USTComboManagingComponent;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

class UMotionWarpingComponent;

UCLASS(config = Game)
class ASTCharacterBase : public ACharacter, 
	public IAbilitySystemInterface, 
	public ISTComboEntityInterface,
	public ISTStateInterface
{
	GENERATED_BODY()

public:
	ASTCharacterBase(const FObjectInitializer& OI);
	

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Called for movement input */
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void ProcessInput(EInputType InputType, const FInputActionInstance& InputInstance);

	UFUNCTION()
	void ProcessWeakAttack(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessGuard(const FInputActionInstance& Instance);


	UFUNCTION()
	void ProcessSway(const FInputActionInstance& Instance);

	void InitializeDefaultSkillSet();



/////////////////////////// Combo Entity Interface
	virtual void SetComboContext(const FComboWindowContext& NewWindow)override;

	virtual void FlushCombo(const FGameplayTagContainer& AllowedTags)override;

	virtual void ClearComboContext()override;
/////////////////////////// State Interface
	void AddState_Implementation(const FGameplayTag& Tag);

	void RemoveState_Implementation(const FGameplayTag& Tag);

	FGameplayTagContainer GetStates_Implementation();

	bool HasState_Implementation(const FGameplayTag& Tag);

	void OnAttributeChanged(const FGameplayAttribute& Attribute, float OldValue, float NewValue);
/////////////////////////////////////////////




	UFUNCTION(BlueprintCallable)
	int32 GetCharacterID() const { return CharacterID; }

	UFUNCTION(BlueprintCallable)
	USTComboManagingComponent* GetComboComponent()const;

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMeshComponent()const;






protected:

	UPROPERTY()
	TObjectPtr<USTComboManagingComponent> ComboComponent;

	UPROPERTY()
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CharacterID;


};

