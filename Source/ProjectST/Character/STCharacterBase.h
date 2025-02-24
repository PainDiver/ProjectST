// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AbilitySystemInterface.h"
#include "GAS/STAbilitySystemComponent.h"
#include "Game/DataAsset/STDataAsset_Input.h"
#include "Character/Component/Combo/STComboEntityInterface.h"
#include "Game/Item/STItemContainerInterface.h"
#include "STCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USTComboManagingComponent;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class ASTCharacterBase : public ACharacter, 
	public IAbilitySystemInterface, 
	public ISTComboEntityInterface
{
	GENERATED_BODY()

public:
	ASTCharacterBase();
	

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

	virtual void FlushCombo()override;

	virtual void ClearComboContext()override;
///////////////////////////
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

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer CharacterState;
};

