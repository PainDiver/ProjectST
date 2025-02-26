// Fill out your copyright notice in the Description page of Project Settings.


#include "STComboManagingComponent.h"
#include "AbilitySystemComponent.h"
#include "Data/DataTableManager.h"
#include "Character/STCharacterBase.h"
#include "ComboContexts.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


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

void USTComboManagingComponent::ProcessCombo(EInputType InputType, const FInputActionInstance& InputInstance)
{
	if (OwnerASC == nullptr)
		return;

	if (UComboContext** CurrentComboContext = ComboContextMap.Find(GetComboContextState(Cast<ASTCharacterBase>(GetOwner()))))
	{
		(*CurrentComboContext)->ProcessCombo(OwnerASC,this,InputType, InputInstance);
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
			if (DataManager->GetTableData(DataManager->RootSkillSetTable,BaseStat.SkillSetDataID, RootSkillSet))
			{
				for (const TPair<EInputType, TSubclassOf<UGameplayAbility>>& RootSkill : RootSkillSet.Abilities)
				{
					if (RootSkill.Key != EInputType::NONE)
						RootComboSet.Add(RootSkill.Key, RootSkill.Value);

					if (GetOwner()->HasAuthority())
					{
						FGameplayAbilitySpec Spec(RootSkill.Value);
						OwnerASC->GiveAbility(Spec);
					}
				}
				ComboInfoCache.DefaultRootSkillSetID = BaseStat.SkillSetDataID;
			}
			ComboInfoCache.CharacterID = CharacterID;
		}
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


	//Jumping
	if (Character && Character->GetCharacterMovement()->MovementMode == MOVE_Falling)
	{
		return EComboContextState::JUMPING;
	}

	//Default
	return EComboContextState::DEFAULT;
}

bool USTComboManagingComponent::SetPendingCombo(const FInputDetail& InputDetail, FGameplayTag& OutGATag)
{
	if (FGameplayTag* FoundGATag = CurrentComboWindow.InputToGA.Find(InputDetail))
	{
		OutGATag = *FoundGATag;
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
		if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager())
		{
			FRootSkillSet RootSkillSet;
			if (DataManager->GetTableData(DataManager->RootSkillSetTable, ComboInfoCache.WeaponRootSkillSetID, RootSkillSet))
			{				
				for (const TPair<EInputType, TSubclassOf<UGameplayAbility>>& RootSkill : RootSkillSet.Abilities)
				{
					if (GetOwner()->HasAuthority())
					{
						if (FGameplayAbilitySpec* Spec = OwnerASC->FindAbilitySpecFromClass(RootSkill.Value))
						{
							OwnerASC->ClearAbility(Spec->Handle);
						}
					}
				}				
			}
		}
	}

	if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager())
	{
		FRootSkillSet RootSkillSet;
		if (DataManager->GetTableData(DataManager->RootSkillSetTable, RootSkillSetID, RootSkillSet))
		{
			for (const TPair<EInputType, TSubclassOf<UGameplayAbility>>& RootSkill : RootSkillSet.Abilities)
			{
				if (RootSkill.Key != EInputType::NONE)
					RootComboSet.Add(RootSkill.Key, RootSkill.Value);

				if (GetOwner()->HasAuthority())
				{
					FGameplayAbilitySpec Spec(RootSkill.Value);
					OwnerASC->GiveAbility(Spec);
				}
			}
		}
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

void USTComboManagingComponent::FlushCombo()
{
	OwnerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(PendingComboTag));	

	// Flush
	PendingComboTag = FGameplayTag();
}

void USTComboManagingComponent::OnRep_ComboInfoCache(const FComboInfoCache& ComboInfo)
{
	SetWeaponRootSkillSet(ComboInfoCache.WeaponRootSkillSetID);
}
