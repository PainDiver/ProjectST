// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/DataAsset/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "Character/Component/Combo/STComboEntityInterface.h"
#include "GameplayTagContainer.h"
#include "STComboManagingComponent.generated.h"


class UAbilitySystemComponent;
class UGameplayAbility;
class ASTCharacterBase;
class UComboContext;

USTRUCT()
struct FComboInfoCache
{
	GENERATED_BODY()

	int32 CharacterID;
	int32 WeaponID;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTComboManagingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTComboManagingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void ProcessCombo(EInputType InputType, const FInputActionInstance& InputInstance);

	// 각 인풋 별 어빌리티를 매핑함
	void Initialize(int CharacterID);

	void InitializeWeaponSkill(int32 WeaponID, int32 OldWeaponID);

	// 상태 판단
	EComboContextState GetComboContextState(ASTCharacterBase* Character)const;

	TMap<EInputType, TSubclassOf<UGameplayAbility>> GetRootComboSet()const { return RootComboSet; }

	bool SetPendingCombo(const FInputDetail& InputDetail,FGameplayTag& OutGATag);

	// 몽타쥬 노티파이스테이트에 의해서 호출될 예정
	UFUNCTION(BlueprintCallable)
	void OpenComboWindow(const FComboWindowContext& NewWindow);

	UFUNCTION(BlueprintCallable)
	void ClearComboWindow();

	//인풋시점과 콤보 실행시점을 분리
	UFUNCTION(BlueprintCallable)
	void FlushCombo();

	FGameplayTag& GetPendingComboTagRef() { return PendingComboTag; }

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<EInputType, TSubclassOf<UGameplayAbility>> RootComboSet;

	UPROPERTY()
	TMap<EComboContextState, UComboContext*> ComboContextMap;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	FComboWindowContext CurrentComboWindow;

	// OpenComboWindow로 인해서 채택된 Ability Tag, 곧 사용되고 Clear예정
	FGameplayTag PendingComboTag;

	FComboInfoCache ComboInfoCache;
};
