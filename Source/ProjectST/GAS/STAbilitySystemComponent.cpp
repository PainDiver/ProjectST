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
		uint32 Count = GameplayTagCountContainer.GetTagCount(Tag);
		AddLooseGameplayTag(Tag,Count+1);
		SetTagMapCount(Tag, Count+ 1 );
		ResolveStateCollapse(Tag);
	}
}

void USTAbilitySystemComponent::RemoveState(const FGameplayTag& Tag)
{
	if (HasState(Tag))
	{
		uint32 Count = GameplayTagCountContainer.GetTagCount(Tag);
		RemoveLooseGameplayTag(Tag, Count-1);
		SetTagMapCount(Tag, Count - 1);
	}
}

void USTAbilitySystemComponent::AddState_Replication(const FGameplayTag& Tag)
{
	if (!HasState(Tag))
	{
		uint32 Count = GameplayTagCountContainer.GetTagCount(Tag);
		AddReplicatedLooseGameplayTag(Tag);
		SetTagMapCount(Tag, Count+1);
		ResolveStateCollapse(Tag);
	}
}

void USTAbilitySystemComponent::RemoveState_Replication(const FGameplayTag& Tag)
{
	if (HasState(Tag))
	{
		uint32 Count = GameplayTagCountContainer.GetTagCount(Tag);
		RemoveReplicatedLooseGameplayTag(Tag);
		SetTagMapCount(Tag, Count-1);
	}
}

bool USTAbilitySystemComponent::HasState(const FGameplayTag& Tag)
{
	return GameplayTagCountContainer.HasMatchingGameplayTag(Tag);
}

void USTAbilitySystemComponent::ResolveStateCollapse(const FGameplayTag& NewTag)
{
	FGameplayTagContainer Tags;
	Tags.AddTag(State_Standing);
	Tags.AddTag(State_Falling);
	Tags.AddTag(State_Lying);
	Tags.AddTag(State_Dead);

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
	return GetOwnedGameplayTags();
}
