// Fill out your copyright notice in the Description page of Project Settings.


#include "STComboManagingComponent.h"
#include "AbilitySystemComponent.h"
#include "Data/DataTableManager.h"
#include "Character/STCharacterBase.h"
#include "ComboContexts.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "Game/STNativeGameplayTag.h"
#include "AbilitySystemBlueprintLibrary.h"


// Sets default values for this component's properties
USTComboManagingComponent::USTComboManagingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USTComboManagingComponent::BeginPlay()
{
	Super::BeginPlay();


	for (uint8 i=0; i < static_cast<uint8>(EComboContextState::MAX); i++)
	{		
		UComboContext* NewComboContext = UComboContext::CreateContext(this,static_cast<EComboContextState>(i));
		ComboContextMap.Add(static_cast<EComboContextState>(i),NewComboContext );
	}
	ComboContextMap.Shrink();
}


// Called every frame
void USTComboManagingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USTComboManagingComponent::ProcessCombo(ESTInputType InputType, const FInputActionInstance& InputInstance, TFunction<void(bool)>&& CallBack)
{
	if (OwnerASC == nullptr)
		return;

	if (UComboContext** CurrentComboContext = ComboContextMap.Find(GetComboContextState(Cast<ASTCharacterBase>(GetOwner()))))
	{
		(*CurrentComboContext)->ProcessCombo(OwnerASC,this,InputType, InputInstance, MoveTemp(CallBack));
	}
}

void USTComboManagingComponent::Initialize(int CharacterID)
{
	if (IAbilitySystemInterface* OwnerAsIASC = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		OwnerASC = OwnerAsIASC->GetAbilitySystemComponent();
	}
	else
	{
		return;
	}

	if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager())
	{
		FCharacterBaseStat BaseStat;
		if (DataManager->GetCharacterStat(CharacterID, BaseStat))
		{
			FRootSkillSet RootSkillSet;
			if (SetRootSkill(BaseStat.SkillSetDataID))
			{
				ComboInfoCache.DefaultRootSkillSetID = BaseStat.SkillSetDataID;
				ComboInfoCache.CharacterID = CharacterID;
			}
		}
	}
}

bool USTComboManagingComponent::SetRootSkill(int32 SkillSetID)
{
	UDataTableManager* DataManager = UDataTableManager::GetDataTableManager();
	if (DataManager == nullptr)
	{
		return false;
	}

	FRootSkillSet RootSkillSet;
	if (DataManager->GetTableData(DataManager->RootSkillSetTable, SkillSetID, RootSkillSet))
	{
		for (const FInputBoundAbility& RootSkill : RootSkillSet.AbilitiesToBind)
		{
			if (RootSkill.InputType != ESTInputType::NONE)
			{
				if (StateRootComboSet.Contains(RootSkill.State))
				{
					StateRootComboSet[RootSkill.State].RootComboSet.Add(RootSkill.InputType,RootSkill.Ability);
				}
				else
				{
					StateRootComboSet.Add(RootSkill.State);
					StateRootComboSet[RootSkill.State].RootComboSet.Add(RootSkill.InputType, RootSkill.Ability);
				}
			}

			if (GetOwner()->HasAuthority())
			{
				FGameplayAbilitySpec Spec(RootSkill.Ability);
				Spec.GameplayEventData = MakeShared<FGameplayEventData>();
				OwnerASC->GiveAbility(Spec);
			}
		}

		for (const TSubclassOf<UGameplayAbility>& GA : RootSkillSet.AbilitiesToGive)
		{
			if (GetOwner()->HasAuthority())
			{
				FGameplayAbilitySpec Spec(GA);
				Spec.GameplayEventData = MakeShared<FGameplayEventData>();
				OwnerASC->GiveAbility(Spec);
			}
		}
		return true;
	}
	return false;
}

void USTComboManagingComponent::ClearRootSkillSet(int32 SkillSetID)
{
	UDataTableManager* DataManager = UDataTableManager::GetDataTableManager();
	if (DataManager == nullptr)
	{
		return;
	}

	FRootSkillSet RootSkillSet;
	if (DataManager->GetTableData(DataManager->RootSkillSetTable, SkillSetID, RootSkillSet))
	{
		for (const FInputBoundAbility& RootSkill : RootSkillSet.AbilitiesToBind)
		{
			if (GetOwner()->HasAuthority())
			{
				if (FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromClass(RootSkill.Ability))
				{
					OwnerASC->ClearAbility(Spec->Handle);
				}
			}
			if (StateRootComboSet.Contains(RootSkill.State))
			{
				StateRootComboSet[RootSkill.State].RootComboSet.Remove(RootSkill.InputType);
			}
		}

		for (const TSubclassOf<UGameplayAbility>& GA : RootSkillSet.AbilitiesToGive)
		{
			if (GetOwner()->HasAuthority())
			{
				if (FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromClass(GA))
				{
					OwnerASC->ClearAbility(Spec->Handle);
				}
			}
		}
	}


	if (GetOwner()->HasAuthority())
	{
		ComboInfoCache.WeaponRootSkillSetID = 0;
	}
}




EComboContextState USTComboManagingComponent::GetComboContextState(ASTCharacterBase* Character) const
{
	if (Character == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character Nullptr So No State"));
		return EComboContextState::MAX;
	}

	//Hit
	if (ISTStateInterface::Execute_HasState(Character,CombatState_Hit))
	{
		return EComboContextState::ON_HIT;
	}

	//Jumping
	if (Character && Character->GetCharacterMovement()->MovementMode == MOVE_Falling ||
		Character->GetCharacterMovement()->MovementMode == MOVE_Flying)
	{
		return EComboContextState::JUMPING;
	}

	//Default
	return EComboContextState::DEFAULT;
}

TMap<ESTInputType, TSubclassOf<UGameplayAbility>> USTComboManagingComponent::GetRootComboSet(EComboContextState State) const
{
	if (StateRootComboSet.Contains(State))
	{
		return StateRootComboSet[State].RootComboSet;
	}
	
	return TMap<ESTInputType, TSubclassOf<UGameplayAbility>>();
}

bool USTComboManagingComponent::SetPendingCombo(const FInputDetail& InputDetail)
{
	if (FGameplayTag* FoundGATag = CurrentComboWindow.InputToGA.Find(InputDetail))
	{
		PendingInput = InputDetail;
		PendingComboTag = *FoundGATag;
		return true;
	}

	return false;
}

void USTComboManagingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 로컬클라 - 서버만
	DOREPLIFETIME_CONDITION_NOTIFY(USTComboManagingComponent, ComboInfoCache,COND_OwnerOnly, REPNOTIFY_OnChanged)
}

void USTComboManagingComponent::SetWeaponRootSkillSet(int32 RootSkillSetID)
{
	if (ComboInfoCache.WeaponRootSkillSetID != 0)
	{
		ClearRootSkillSet(ComboInfoCache.WeaponRootSkillSetID);
	}
	
	if (SetRootSkill(RootSkillSetID))
	{
		ComboInfoCache.WeaponRootSkillSetID = RootSkillSetID;
	}
}

void USTComboManagingComponent::OpenComboWindow(const FComboWindowContext& NewWindow)
{	
	CurrentComboWindow.Set(NewWindow);
}

void USTComboManagingComponent::ClearComboWindow()
{
	CurrentComboWindow.Reset();
}

bool USTComboManagingComponent::FlushCombo(const FGameplayTagContainer& AllowedTags)
{
	if (PendingComboTag != FGameplayTag::EmptyTag)
	{
		if (AllowedTags.IsEmpty() || AllowedTags.HasTag(PendingComboTag))
		{
			UComboInputData* ComboInput = NewObject<UComboInputData>();
			ComboInput->InputType = PendingInput.InputType;
			ComboInput->InputInstance = PendingInput.Instance;
			TSharedPtr<FGameplayEventData> GameplayEventData = MakeShared<FGameplayEventData>();
			GameplayEventData->OptionalObject2 = ComboInput;
			OwnerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(PendingComboTag));
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerASC->GetAvatarActor(), Input_InputInstance, *GameplayEventData);

			PendingComboTag = FGameplayTag::EmptyTag;
			PendingInput.Clear();
			return true;
		}
	}

	return false;
}

void USTComboManagingComponent::OnRep_ComboInfoCache(const FComboInfoCache& ComboInfo)
{
	ClearRootSkillSet(ComboInfo.WeaponRootSkillSetID);

	SetWeaponRootSkillSet(ComboInfoCache.WeaponRootSkillSetID);

	OnComboInfoChanged.Broadcast(ComboInfo);
}
