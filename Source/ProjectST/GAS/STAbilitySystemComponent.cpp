// Fill out your copyright notice in the Description page of Project Settings.


#include "STAbilitySystemComponent.h"
#include "STAttributeSet.h"
#include "Game/STNativeGameplayTag.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void USTAbilitySystemComponent::Initialize(AActor* Owner, AActor* Avatar, uint32 CharacterID,TFunction<void()>&& OnInitCallBack)
{

	InitAbilityActorInfo(Owner,Avatar);

	if (Owner->HasAuthority())
	{
		const USTAttributeSet* Set = AddSet<USTAttributeSet>();
		const_cast<USTAttributeSet*>(Set)->OnInitializeAttributes(CharacterID);
	}

	OnInitCallBack();

//	FGameplayAbilitySpec()
//	GiveAbility

	if (ACharacter* Character = Cast<ACharacter>(Avatar))
	{
		if (UCharacterMovementComponent* CM = Character->GetCharacterMovement())
		{
			if (CM->IsFalling())
			{
				AddState(State_Falling);
			}
			else
			{
				AddState(State_Standing);
			}
		}
	}
}

void USTAbilitySystemComponent::AddState(const FGameplayTag& Tag)
{
	if (!HasState(Tag))
	{
		AddLooseGameplayTag(Tag);
		ResolveStateCollapse(Tag);
	}
}

void USTAbilitySystemComponent::RemoveState(const FGameplayTag& Tag)
{
	if (HasState(Tag))
	{
		RemoveLooseGameplayTag(Tag);
	}
}

void USTAbilitySystemComponent::AddState_Replication(const FGameplayTag& Tag)
{
	AddReplicatedLooseGameplayTag(Tag);
}

void USTAbilitySystemComponent::RemoveState_Replication(const FGameplayTag& Tag)
{
	RemoveReplicatedLooseGameplayTag(Tag);
}


bool USTAbilitySystemComponent::HasState(const FGameplayTag& Tag)
{
	if (GetReplicatedLooseTags().TagMap.Contains(Tag))
	{
		return true;
	}
	return GameplayTagCountContainer.HasMatchingGameplayTag(Tag);
}

void USTAbilitySystemComponent::ResolveStateCollapse(const FGameplayTag& NewTag)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(State_Standing);
	Tags.AddTag(State_Falling);
	Tags.AddTag(State_Lying);

	if (Tags.HasTag(NewTag))
	{
		Tags.RemoveTag(NewTag);

		for (const FGameplayTag& Tag : Tags)
		{
			RemoveState(Tag);
		}
	}
}

FGameplayTagContainer USTAbilitySystemComponent::GetStates()
{
	FGameplayTagContainer Container = GameplayTagCountContainer.GetExplicitGameplayTags();
	for (auto& Pair : GetReplicatedLooseTags().TagMap)
	{
		if (Pair.Value > 0)
		{
			Container.AddTag(Pair.Key);
		}
	}
	return Container;
}
