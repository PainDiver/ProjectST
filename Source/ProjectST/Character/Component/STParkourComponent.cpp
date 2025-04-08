// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STParkourComponent.h"
#include "Character/STStateInterface.h"
#include "GAS/GA/STGameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Misc/STEventManager.h"

UWorld* UParkourState::GetWorld() const
{
	UWorld* World = nullptr;
#if WITH_EDITOR
	World = GEditor->PlayWorld;
#else
	World = Super::GetWorld();
#endif
	return World;
}

// Sets default values for this component's properties
USTParkourComponent::USTParkourComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USTParkourComponent::BeginPlay()
{
	Super::BeginPlay();

	ManagedParkours.Shrink();

	
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (Pawn && Pawn->IsLocallyControlled())
	{
		FSTEventDelegate Delegate;
		Delegate.BindDynamic(this, &USTParkourComponent::Initialize);
		USTEventManager::GetEventManager()->RegisterEvent_Subject(GetOwner(), Event_CharacterPrepared, Delegate, nullptr, true);
	}
}

void USTParkourComponent::Initialize(UObject* Data)
{
	// 로컬 제외 다른주체가 하기에는 너무 무거운일을 하게될거니까 로컬에 의존해야할듯..
	if (IAbilitySystemInterface* StateInterface = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		FOnGameplayEffectTagCountChanged& Delegate = StateInterface->GetAbilitySystemComponent()->RegisterGenericGameplayTagEvent();
		Delegate.Add(FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &USTParkourComponent::CheckParkourStateTag));
	}
}


void USTParkourComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (UParkourState* ParkourState : ParkourStateOnRunning)
	{
		if (ParkourState == nullptr)
			continue;

		TArray<FVector> ParkourPoints;
		TArray<UPrimitiveComponent*> WallsToIgnore;
		if (!IsParkourInProgress() && ParkourState->PrepareParkour(GetOwner(), DeltaTime,ParkourPoints,WallsToIgnore))
		{			
			SetParkourProgress(true);
			ParkourState->DoParkour(GetOwner(),ParkourPoints,WallsToIgnore);
			break;			
		}
	}

}

void USTParkourComponent::CheckParkourStateTag(const FGameplayTag Tag, int32 Count)
{
	FGameplayTagContainer StateContainer = ISTStateInterface::Execute_GetStates(GetOwner());

	for (TPair<FGameplayTag, UParkourState*>& Pair : ManagedParkours)
	{
		if (Pair.Value == nullptr)
			continue;

		if (StateContainer.HasAll(Pair.Value->GetLinkedManagedTags()))
		{
			Pair.Value->OnStartPreparing(GetOwner());
			ParkourStateOnRunning.Add(Pair.Value);
		}
		else
		{
			Pair.Value->OnEndPreparing(GetOwner());
			ParkourStateOnRunning.Remove(Pair.Value);
		}
	}

}
