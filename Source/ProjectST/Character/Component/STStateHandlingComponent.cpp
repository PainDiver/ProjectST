// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STStateHandlingComponent.h"
#include "ManagedStates/STManagedStates.h"
#include "Engine/ActorChannel.h"
#include "Character/STStateInterface.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"

#include "Misc/STEventManager.h"
#include "Game/STNativeGameplayTag.h"

// Sets default values for this component's properties
USTStateHandlingComponent::USTStateHandlingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTStateHandlingComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = GetOwner();	
	if (Owner->HasAuthority())
	{
		FSTEventDelegate Delegate;
		Delegate.BindDynamic(this, &ThisClass::Initialize);
		USTEventManager::GetEventManager()->RegisterEvent_Subject(Owner, Event_CharacterPrepared, MoveTemp(Delegate), nullptr, true);
	}
	States.Shrink();
	StateOnRunning.Shrink();
}

void USTStateHandlingComponent::Initialize(UObject* Data)
{
	if (IAbilitySystemInterface* IASC = Cast<IAbilitySystemInterface>(Owner))
	{
		if (UAbilitySystemComponent* ASC = IASC->GetAbilitySystemComponent())
		{
			ASC->RegisterGenericGameplayTagEvent().Add(FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnAnyTagCountChanged));
			for (USTManagedState* ManagedState : States)
			{
				FOnGameplayEffectTagCountChanged& Delegate = ASC->RegisterGameplayTagEvent(ManagedState->GetTag(), EGameplayTagEventType::AnyCountChange);
				Delegate.Add(FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ThisClass::OnManagedTagCountChanged));
			}
		}
	}
}

// Called every frame
void USTStateHandlingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (USTManagedState* State : States)
	{
		if (State == nullptr)
			continue;

		if (CurrentRunningTag.HasTag(State->GetTag()))
		{
			State->OnTick(Owner,this,DeltaTime);
		}		
	}

}

void USTStateHandlingComponent::RestoreLastStateEffect()
{
	if (StateOnRunning.Num() > 0)
	{
		const FGameplayTag& Tag = StateOnRunning.Last();
		USTManagedState** MatchedState = States.FindByPredicate([Tag](USTManagedState* State)
			{
				return State->GetTag() == Tag;
			});

		if (MatchedState)
		{
			(*MatchedState)->OnStateAdded(GetOwner(),this);
		}
	}
}

bool USTStateHandlingComponent::CanAddState(const FGameplayTag& Tag)
{
	USTManagedState** FoundState = States.FindByPredicate([Tag](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == Tag;
			}
			return false;
		});

	if (FoundState)
	{
		return (*FoundState)->CanAddState(GetOwner());
	}

	return true;
}

void USTStateHandlingComponent::OnManagedTagCountChanged(FGameplayTag Tag, int32 Count)
{
	USTManagedState** State = States.FindByPredicate(
		[Tag](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == Tag;
			}
			return false;
		}
	);

	if (State == nullptr)
	{
		return;
	}

	if (Count > 0)
	{
		OnStateAdded_Multi((*State)->GetTag());
	}
	else
	{
		OnStateRemoved_Multi((*State)->GetTag());
	}
		
}

void USTStateHandlingComponent::OnAnyTagCountChanged(FGameplayTag Tag, int32 Count)
{
	for (USTManagedState* ManagedState : States)
	{
		if(!ManagedState->IsRemoved())
			ManagedState->ResolveCollapsingStates(GetOwner(),this);
	}
}

void USTStateHandlingComponent::AddManagedState(const FGameplayTag& AddedState)
{
	USTManagedState** State = States.FindByPredicate(
		[AddedState](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == AddedState;
			}
			return false;
		}
	);

	if (!CanReigsterManagedState(*State))
	{
		return;
	}

	if (State)
	{
		(*State)->OnStateAdded(Owner, this);
		StateOnRunning.AddUnique((*State)->GetTag());
		CurrentRunningTag.AddTag(AddedState);
	}
}
void USTStateHandlingComponent::OnStateAdded_Multi_Implementation(const FGameplayTag& AddedState)
{
	AddManagedState(AddedState);
}

void USTStateHandlingComponent::RemoveManagedState(const FGameplayTag& RemoveState)
{
	USTManagedState** State = States.FindByPredicate(
		[RemoveState](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == RemoveState;
			}
			return false;
		}
	);

	if (!CanReigsterManagedState(*State))
	{
		return;
	}

	if (State)
	{
		(*State)->OnStateRemoved(Owner, this);
		StateOnRunning.Remove((*State)->GetTag());
		CurrentRunningTag.RemoveTag(RemoveState);
	}
}

void USTStateHandlingComponent::OnStateRemoved_Multi_Implementation(const FGameplayTag& RemoveState)
{
	RemoveManagedState(RemoveState);
}

bool USTStateHandlingComponent::CanReigsterManagedState(USTManagedState* ManagedState)
{
	ENetMode NetMode = GetNetMode();
	if (NetMode == ENetMode::NM_Standalone)
		return true;

	APawn* Pawn = Cast<APawn>(Owner);
	switch (ManagedState->GetRealm())
	{
		case EManagedStateRealm::All:
		{
			return true;
		}
		case EManagedStateRealm::Server:
		{
			return Owner->HasAuthority();
		}
		case EManagedStateRealm::OnlyClients:
		{
			return NetMode == ENetMode::NM_Client || 
				(NetMode == ENetMode::NM_ListenServer && Pawn->HasAuthority());
		}
		case EManagedStateRealm::OwnerClientOnly:
		{
			return Pawn->IsLocallyControlled();
		}
		case EManagedStateRealm::ExceptRemote :
		{
			return Pawn->IsLocallyControlled() || Owner->HasAuthority();
		}
		default:
			break;
	}

	return false;
}

USTManagedState* USTStateHandlingComponent::FindState(const FGameplayTag& Tag)
{
	USTManagedState** State = States.FindByPredicate(
		[Tag](USTManagedState* State)
		{
			if (State)
			{
				return State->GetTag() == Tag;
			}
			return false;
		}
	);

	return *State;
}
