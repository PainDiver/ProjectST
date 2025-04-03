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
class USTMotionWarpingComponent;
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
	
	virtual void BeginPlay()override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION()
	void OnPreparedBothSide(UObject* Data);

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintNativeEvent, DisplayName = "OnHealthChanged")
	void K2_OnHealthChanged(float OldValue,float NewValue,AActor* EffectInstigator);
	void K2_OnHealthChanged_Implementation(float OldValue, float NewValue, AActor* EffectInstigator) {};

	void OnStaminaChanged(const FOnAttributeChangeData& Data);
	UFUNCTION(BlueprintNativeEvent, DisplayName = "OnStaminaChanged")
	void K2_OnStaminaChanged(float OldValue, float NewValue);
	void K2_OnStaminaChanged_Implementation(float OldValue, float NewValue) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityCommitted(UGameplayAbility* GA);
	void OnAbilityCommitted_Implementation(UGameplayAbility* GA) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityCommitFailed(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag);
	void OnAbilityCommitFailed_Implementation(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag) {};


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

	UFUNCTION()
	void ProcessSkillQ(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessSkillE(const FInputActionInstance& Instance);


	void InitializeDefaultSkillSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

/////////////////////////// Combo Entity Interface

	TMap<ESTInputType, TSubclassOf<UGameplayAbility>> GetRootComboSet_Implementation(EComboContextState State)const;

	virtual void SetComboContext(const FComboWindowContext& NewWindow)override;

	virtual bool FlushCombo(const FGameplayTagContainer& AllowedTags)override;

	virtual void ClearComboContext()override;
/////////////////////////// State Interface

	int32 GetCharacterID_Implementation()override {return CharacterID;};

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

	bool IsImmortalState_Implementation();

	bool HasState_Implementation(const FGameplayTag& Tag);

	void OnAttributeChanged_Implementation(const FGameplayAttribute& Attribute, float OldValue, float NewValue)override;

/////////////////////////////////////////////


	UFUNCTION(BlueprintCallable)
	USTComboManagingComponent* GetComboComponent()const;

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMeshComponent()const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetWeaponType(EWeaponType Type) { CurrentWeaponType = Type; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponType GetWeaponType()const{ return CurrentWeaponType; }




protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTComboManagingComponent> ComboComponent;

	UPROPERTY()
	TObjectPtr<USTAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<USTMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTStateHandlingComponent> StateHandlingComponent;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CharacterID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated)
	EWeaponType CurrentWeaponType;

};

