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
class USTStateHandlingComponent;
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

	virtual bool CanJumpInternal_Implementation() const override;

	void ProcessInput(ESTInputType InputType, const FInputActionInstance& InputInstance,TFunction<void(bool)>&& CallBack = TFunction<void(bool)>());

	UFUNCTION()
	void ProcessWeakAttack(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessGuard(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessSway(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessSprint(const FInputActionInstance& Instance);


	void InitializeDefaultSkillSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

/////////////////////////// Combo Entity Interface
	virtual void SetComboContext(const FComboWindowContext& NewWindow)override;

	virtual void FlushCombo(const FGameplayTagContainer& AllowedTags)override;

	virtual void ClearComboContext()override;
/////////////////////////// State Interface
	bool AddState_Implementation(const FGameplayTag& Tag);

	void RemoveState_Implementation(const FGameplayTag& Tag);

	bool AddState_Replication_Implementation(const FGameplayTag& Tag);

	void RemoveState_Replication_Implementation(const FGameplayTag& Tag);

	void OnDead_Implementation(AActor* Killer)override;

	void OnKill_Implementation(AActor* Killed)override;
	
	UFUNCTION(BlueprintCallable,NetMulticast,Reliable)
	void BroadCastDead_Multicast(AActor* Killer);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void BroadCastKill_Multicast(AActor* Killed);

	FGameplayTagContainer GetStates_Implementation();

	bool HasState_Implementation(const FGameplayTag& Tag);

	void OnAttributeChanged_Implementation(const FGameplayAttribute& Attribute, float OldValue, float NewValue)override;

/////////////////////////////////////////////


	UFUNCTION(BlueprintCallable)
	FORCEINLINE int32 GetCharacterID() const { return CharacterID; }

	UFUNCTION(BlueprintCallable)
	USTComboManagingComponent* GetComboComponent()const;

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMeshComponent()const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetWeaponType(EWeaponType Type) { CurrentWeaponType = Type; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType()const{ return CurrentWeaponType; }


protected:

	UPROPERTY()
	TObjectPtr<USTComboManagingComponent> ComboComponent;

	UPROPERTY()
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTStateHandlingComponent> StateHandlingComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CharacterID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated)
	EWeaponType CurrentWeaponType;

};

