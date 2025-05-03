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

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << WeaponRootSkillSetID;
		return !Ar.IsError();
	}

	UPROPERTY()
	int32 CharacterID;
	
	UPROPERTY()
	int32 DefaultRootSkillSetID;

	UPROPERTY()
	int32 WeaponRootSkillSetID;
};

template<>
struct TStructOpsTypeTraits<FComboInfoCache> : public TStructOpsTypeTraitsBase2<FComboInfoCache>
{
	enum
	{
		WithNetSerializer = true,
	};
};

USTRUCT(BlueprintType,Blueprintable)
struct FRootComboSet
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<ESTInputType, TSubclassOf<UGameplayAbility>> RootComboSet;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboInfoChanged, const FComboInfoCache&, NewComboInfo);

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


	void ProcessCombo(ESTInputType InputType, const FInputActionInstance& InputInstance,TFunction<void(bool)>&& CallBack);

	// 각 인풋 별 어빌리티를 매핑함
	void Initialize(int CharacterID);

	bool SetRootSkill(int32 SkillSetID);

	UFUNCTION(BlueprintCallable)
	void ClearRootSkillSet(int32 SkillSetID);

	// 상태 판단
	EComboContextState GetComboContextState(ASTCharacterBase* Character)const;

	UFUNCTION(BlueprintCallable)
	TMap<ESTInputType, TSubclassOf<UGameplayAbility>> GetRootComboSet(EComboContextState State)const;

	bool SetPendingCombo(const FInputDetail& InputDetail);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Server에서 사용시켜야함
	UFUNCTION(BlueprintCallable)
	void SetWeaponRootSkillSet(int32 RootSkillSetID);


	// 몽타쥬 노티파이스테이트에 의해서 호출될 예정
	UFUNCTION(BlueprintCallable)
	void OpenComboWindow(const FComboWindowContext& NewWindow);

	UFUNCTION(BlueprintCallable)
	void ClearComboWindow();

	//인풋시점과 콤보 실행시점을 분리
	UFUNCTION(BlueprintCallable)
	bool FlushCombo(const FGameplayTagContainer& AllowedTags);

	UFUNCTION()
	void OnRep_ComboInfoCache(const FComboInfoCache& ComboInfo);

	UPROPERTY(BlueprintAssignable)
	FOnComboInfoChanged OnComboInfoChanged;


private:

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	//TMap<ESTInputType, TSubclassOf<UGameplayAbility>> RootComboSet;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<EComboContextState, FRootComboSet> StateRootComboSet;

	UPROPERTY()
	TMap<EComboContextState, UComboContext*> ComboContextMap;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;

	FComboWindowContext CurrentComboWindow;

	// OpenComboWindow로 인해서 채택된 Ability Tag, 곧 사용되고 Clear예정
	FGameplayTag PendingComboTag;

	FInputDetail PendingInput;

	UPROPERTY(ReplicatedUsing = OnRep_ComboInfoCache)
	FComboInfoCache ComboInfoCache;
};
