// Fill out your copyright notice in the Description page of Project Settings.


#include "STAbilitySystemComponent.h"
#include "STAttributeSet.h"

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


}

