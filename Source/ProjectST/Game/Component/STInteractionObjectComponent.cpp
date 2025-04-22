// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Component/STInteractionObjectComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"
#include "Character/STPlayerInterface.h"
#include "Character/STCharacterInterface.h"
#include "Character/Component/STInteractionSubjectComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
USTInteractionObjectComponent::USTInteractionObjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void USTInteractionObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractionDelegates.Reserve(InteractionInfos.Num());
	for (int32 i=0; i < InteractionInfos.Num();i++)
	{
		// 순서에 의해서 Delegate가 먼저들어올수도있기때문
		if(!InteractionDelegates.IsValidIndex(i))
			InteractionDelegates.AddDefaulted();

		InteractionContexts.Add({ InteractionInfos[i].interactionMaxCount });
	}
}


// Called every frame
void USTInteractionObjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (InteractionProgress.IsOnInteraction())
	{
		if (CanInteractOnTick(InteractionProgress.Interactor))
		{
			OnTickInteraction(InteractionProgress.Interactor, DeltaTime);			
			if (GetOwner()->HasAuthority())
			{
				if(InteractionProgress.ElapsedInteractionTime >= InteractionProgress.CurrentInteractionInfo->InteractionTime)
				{
					EndInteraction(InteractionProgress.Interactor, true);
				}
			}
		}
		else
		{
			if (GetOwner()->HasAuthority() || InteractionProgress.Interactor->IsLocallyControlled())
			{
				EndInteraction(InteractionProgress.Interactor,false);
			}
		}
	}
}

bool USTInteractionObjectComponent::CanInteract(AActor* NewInteractor, int32 Index)
{	
	if (InteractionDelegates[Index].CanInteractDelegate.IsBound() && !InteractionDelegates[Index].CanInteractDelegate.Execute(NewInteractor, InteractionInfos[Index]))
	{
		return false;
	}

	if (InteractionContexts[Index].RemainingCount <= 0)
	{
		return false;
	}

	return !InteractionProgress.IsOnInteraction();
}

void USTInteractionObjectComponent::OnInteractionFail(AActor* Interactor, int32 Index)
{
	OnFailInteractionDelegate.Broadcast(Interactor, InteractionInfos[Index]);
}

void USTInteractionObjectComponent::StartInteraction(AActor* NewInteractor,int32 Index)
{	
	if (!InteractionInfos.IsValidIndex(Index))
	{
		return;
	}

	if (!CanInteract(NewInteractor, Index))
	{
		OnInteractionFail(NewInteractor, Index);
		return;	
	}

	InteractionProgress.Set(Cast<ACharacter>(NewInteractor),NewInteractor->GetComponentByClass<USTInteractionSubjectComponent>(), Index, &InteractionInfos[Index], &InteractionDelegates[Index]);
	InteractionProgress.CurrentInteractionDelegates->OnStartInteractionDelegate.ExecuteIfBound(NewInteractor, *InteractionProgress.CurrentInteractionInfo);

	if (InteractionProgress.CurrentInteractionInfo->InteractionTime >= 0.02f)
	{
		SetComponentTickEnabled(true);
	}
	else
	{
		EndInteraction(NewInteractor, true);
	}
}

bool USTInteractionObjectComponent::CanInteractOnTick(AActor* Interactor)
{
	if (GetOwner()->GetLocalRole() == ENetRole::ROLE_SimulatedProxy)
		return true;

	if (InteractionProgress.CurrentInteractionDelegates->CanInteractOnTickDelegate.IsBound() &&
		!InteractionProgress.CurrentInteractionDelegates->CanInteractOnTickDelegate.Execute(Interactor, *InteractionProgress.CurrentInteractionInfo))
	{
		return false;
	}
	

	return true;
}

void USTInteractionObjectComponent::OnTickInteraction(AActor* Interactor,float DeltaTime)
{
	InteractionProgress.ElapsedInteractionTime += DeltaTime;
	InteractionProgress.CurrentInteractionDelegates->OnTickDelegate.ExecuteIfBound(Interactor, DeltaTime, *InteractionProgress.CurrentInteractionInfo);
}

void USTInteractionObjectComponent::EndInteraction(AActor* Interactor, bool bIsSuccess)
{
	if (GetOwner()->HasAuthority())
	{
		if (!IsInteracting())
			return;

		EndInteraction_Multi(Interactor, bIsSuccess);
	}
	else if (InteractionProgress.Interactor->IsPlayerControlled())
	{
		if (!IsInteracting())
			return;

		EndInteraction_Server(Interactor, bIsSuccess);
	}
}

void USTInteractionObjectComponent::EndInteraction_Server_Implementation(AActor* Interactor, bool bIsSuccess)
{
	EndInteraction_Multi(Interactor, bIsSuccess);
}

void USTInteractionObjectComponent::EndInteraction_Multi_Implementation(AActor* Interactor, bool bIsSuccess)
{
	if (!IsInteracting())
		return;

	if (bIsSuccess)
	{
		InteractionContexts[InteractionProgress.InteractionIndex].RemainingCount--;
	}

	InteractionProgress.CurrentInteractionDelegates->OnEndInteractionDelegate.ExecuteIfBound(Interactor, bIsSuccess, *InteractionProgress.CurrentInteractionInfo);
	InteractionProgress.OnInteractionDone();
	InteractionProgress.InterectionSubjectComponent->OnScanNewInteractableObject(nullptr);
	InteractionProgress.Reset();
	SetComponentTickEnabled(false);
}

void USTInteractionObjectComponent::SetCurrentSelectedIndex(int32 Index)
{
	int32 TempCurrentIndex = CurrentSelectedIndex;
	CurrentSelectedIndex = Index;
	OnInteractionIndexSelectedDelegate.Broadcast(TempCurrentIndex,Index);
}

FInteractionDelegateSets& USTInteractionObjectComponent::GetInteractionDelegates(int32 Index)
{
	if (!InteractionDelegates.IsValidIndex(Index))
	{
		for (int i = 0; i< Index+1; i++)
		{
			if(!InteractionDelegates.IsValidIndex(i))
				InteractionDelegates.AddDefaulted();
		}
	}
	return InteractionDelegates[Index];
}

