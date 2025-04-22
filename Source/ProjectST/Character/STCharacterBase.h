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
#include "Character/STCharacterInterface.h"
#include "Game/Component/STInteractionObjectComponent.h"
#include "STCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class USTComboManagingComponent;
class USTStateHandlingComponent;
class USTMotionWarpingComponent;
class USTInteractionObjectComponent;
struct FInputActionValue;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

class UMotionWarpingComponent;
class USTParkourComponent;
class USTInteractionSubjectComponent;

UCLASS(config = Game)
class ASTCharacterBase : public ACharacter, 
	public IAbilitySystemInterface, 
	public ISTComboEntityInterface,
	public ISTStateInterface,
	public ISTCharacterInterface
{
	GENERATED_BODY()

public:
	ASTCharacterBase(const FObjectInitializer& OI);
	
	virtual void BeginPlay()override;

	virtual void BeginDestroy()override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	void PointAbilitySystemComponent(USTAbilitySystemComponent* ActualASC);

	UFUNCTION()
	void OnPreparedBothSide(UObject* Data);

	void OnHealthChanged(const FOnAttributeChangeData& Data);

	void OnStaminaChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityCommitted(UGameplayAbility* GA);
	void OnAbilityCommitted_Implementation(UGameplayAbility* GA) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnAbilityCommitFailed(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag);
	void OnAbilityCommitFailed_Implementation(const UGameplayAbility* GA, const FGameplayTagContainer& ExplainingTag) {};

	virtual bool CanJumpInternal_Implementation() const override;

	UFUNCTION(BlueprintNativeEvent)
	bool CanProcessInput(ESTInputType InputType, const FInputActionInstance& InputInstance) const;
	bool CanProcessInput_Implementation(ESTInputType InputType, const FInputActionInstance& InputInstance)const { return true; };


	void ProcessInput(ESTInputType InputType, const FInputActionInstance& InputInstance,TFunction<void(bool)>&& CallBack = TFunction<void(bool)>());

	UFUNCTION()
	void ProcessMove(const FInputActionInstance& Value);

	UFUNCTION()
	void ProcessLook(const FInputActionInstance& Value);

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

	UFUNCTION()
	void ProcessSkillR(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessLockOn(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessInteraction(const FInputActionInstance& Instance);

	UFUNCTION()
	void ProcessSelectInteraction(const FInputActionInstance& Instance);

	/*UFUNCTION(BlueprintNativeEvent)
	void OnProcessLockOn(const FInputActionInstance& Instance);
	void OnProcessLockOn_Implementation(const FInputActionInstance& Instance) {};*/

	UFUNCTION(BlueprintCallable)
	USTInventoryComponent* CloneInventoryAndHave(USTInventoryComponent* Inventory, bool bManualAttachment, const FTransform& RelativeTransform);

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
	
	FGameplayTagContainer GetStates_Implementation();

	bool IsImmortalState_Implementation();

	bool HasState_Implementation(const FGameplayTag& Tag);

	void OnAttributeChanged_Implementation(const FGameplayAttribute& Attribute, float OldValue, float NewValue)override {}

	virtual void BindOnStateAddedDelegate(const FGameplayTag& Tag, FOnStateAddedDelgate::FDelegate&& Delegate)override;
	virtual void BindOnStateTickDelegate(const FGameplayTag& Tag, FOnStateTickDelgate::FDelegate&& Delegate)override;
	virtual void BindOnStateRemoveDelegate(const FGameplayTag& Tag, FOnStateRemovedDelgate::FDelegate&& Delegate)override;

/////////////////////////////////////////////


	UFUNCTION(BlueprintCallable)
	USTComboManagingComponent* GetComboComponent()const;

	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetMeshComponent()const;

//////////////////// Character Interface
	void SetWeaponType_Implementation(EWeaponType Type) { CurrentWeaponType = Type; }

	EWeaponType GetWeaponType_Implementation()const{ return CurrentWeaponType; }

	AActor* GetCurrentScannedActor_Implementation()const;

	USTInventoryComponent* GetInventoryComponent_Implementation()const;

	ASTPlayerState* GetSTPlayerState_Implementation()const;

////////////////////


protected:

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTComboManagingComponent> ComboComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTMotionWarpingComponent> MotionWarpingComponent;

	UPROPERTY(BlueprintReadOnly,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTStateHandlingComponent> StateHandlingComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTParkourComponent> ParkourComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTInteractionSubjectComponent> InteractionSubjectComp;

	// 나중에 시체될때 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTInteractionObjectComponent> InteractionObjectComp;


	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 CharacterID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Replicated)
	EWeaponType CurrentWeaponType;

	UPROPERTY()
	TObjectPtr<USTAbilitySystemComponent> ASC_Pointer;
};

