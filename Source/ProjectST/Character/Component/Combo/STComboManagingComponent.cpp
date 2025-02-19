// Fill out your copyright notice in the Description page of Project Settings.


#include "STComboManagingComponent.h"
#include "AbilitySystemComponent.h"
#include "Game/DataTableManager.h"
#include "Character/STCharacterBase.h"
#include "ComboContexts.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USTComboManagingComponent::USTComboManagingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

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

	if (UComboContext* CurrentComboContext = ComboContextMap[GetComboContextState(Cast<ASTCharacterBase>(GetOwner()))])
	{
		CurrentComboContext->ProcessCombo(OwnerASC,this,InputType, InputInstance);
	}
}

void USTComboManagingComponent::Initialize(int CharacterID)
{
	if (IAbilitySystemInterface* OwnerAsIASC = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		OwnerASC = OwnerAsIASC->GetAbilitySystemComponent();
	}

	if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager(GetOwner()))
	{
		FCharacterBaseStat BaseStat;
		if (DataManager->GetCharacterStat(CharacterID, BaseStat))
		{
			FRootSkillSet RootSkillSet;
			if (DataManager->GetRootSkillSet(BaseStat.ComboDataID, RootSkillSet))
			{
				for (const TPair<EInputType, TSubclassOf<UGameplayAbility>>& RootSkill : RootSkillSet.Abilities)
				{
					RootComboSet.Add(RootSkill.Key, RootSkill.Value);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("RootSkillSet Not Found Data ID : %d"), BaseStat.ComboDataID);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("CharacterStat Not Found Data ID : %d"),CharacterID);
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

void USTComboManagingComponent::OpenComboWindow(const FComboWindowContext& NewWindow)
{
	CurrentComboWindow = NewWindow;		
}

void USTComboManagingComponent::ClearComboWindow()
{
	CurrentComboWindow.Reset();
}

void USTComboManagingComponent::StartCombo()
{
	OwnerASC->TryActivateAbilitiesByTag(FGameplayTagContainer(PendingComboTag));	
}

