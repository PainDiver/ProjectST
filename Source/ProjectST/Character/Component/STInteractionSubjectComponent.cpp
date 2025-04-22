// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STInteractionSubjectComponent.h"
#include "Game/Component/STInteractionObjectComponent.h"
#include "Misc/STGameBlueprintFunctionLibrary.h"

// Sets default values for this component's properties
USTInteractionSubjectComponent::USTInteractionSubjectComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void USTInteractionSubjectComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterOwner = Cast<ACharacter>(GetOwner());
	if (CharacterOwner && CharacterOwner->GetController() && CharacterOwner->GetController()->IsLocalPlayerController())
	{
		PlayerController = Cast<APlayerController>(CharacterOwner->GetController());
		GetWorld()->GetTimerManager().SetTimer(
			ScanHandle,
			FTimerDelegate::CreateUObject(this, &ThisClass::ProcessScan)
			, ScanFrequency,
			true
		);
	}

}


// Called every frame
void USTInteractionSubjectComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


bool USTInteractionSubjectComponent::CanScan()
{	
	if (CanScanDelegate.IsBound() && !CanScanDelegate.Execute())
	{
		return false;
	}

	return true;
}

void USTInteractionSubjectComponent::ProcessScan()
{
	if (!CanScan())
	{
		if( CurrentInteractionObjectComp == nullptr || !CurrentInteractionObjectComp->IsInteracting())
		{
			OnScanNewInteractableObject(nullptr);
			return;
		}
		return;
	}

	TArray<FHitResult> ScanResults = USTGameBlueprintFunctionLibrary::ConeTraceByObjectTypes
	(
		CharacterOwner,
		PlayerController->PlayerCameraManager->GetCameraLocation(),
		CharacterOwner->GetControlRotation().Vector(),
		ScanDistance,
		ScanAngle,
		false,
		ObjectsToScan,
		EConeTraceSortOption::ClosestAngle,
		false,
		bDebugScan);

	if (ScanResults.Num() > 0)
	{
		AActor* ClosestActor = ScanResults[0].GetActor();
		if (ClosestActor != CurrentScannedActor)
		{
			OnScanNewInteractableObject(ClosestActor);
		}
	}
	else if(CurrentScannedActor != nullptr || (bHadScannedActor && CurrentScannedActor == nullptr))
	{
		OnScanNewInteractableObject(nullptr);
	}

	bHadScannedActor = CurrentScannedActor != nullptr;
}

void USTInteractionSubjectComponent::OnScanNewInteractableObject(AActor* New)
{		
	if (CurrentInteractionObjectComp && CurrentInteractionObjectComp->IsInteracting())
	{
		EndInteraction(false);
	}
	
	AActor* OldScannedActor = CurrentScannedActor;
	CurrentScannedActor = New;	
	if (CurrentScannedActor)
	{
		CurrentInteractionObjectComp = CurrentScannedActor->GetComponentByClass<USTInteractionObjectComponent>();
	}
	else
	{
		CurrentInteractionObjectComp = nullptr;
	}
	OnScanNewInteractableObjectDelegate.Broadcast(OldScannedActor, New);	
}

void USTInteractionSubjectComponent::ProcessInteraction()
{
	if (CurrentScannedActor == nullptr)
		return;

	// Input -> 서버 RPC 필요
	ProcessInteraction_Server(CurrentScannedActor, CurrentInteractionObjectComp->GetCurrentSelectedIndex());
}

void USTInteractionSubjectComponent::ProcessInteraction_Server_Implementation(AActor* ScannedActor,int32 Index)
{
	ProcessInteraction_Multi(ScannedActor,Index);
}

void USTInteractionSubjectComponent::ProcessInteraction_Multi_Implementation(AActor* ScannedActor, int32 Index)
{
	if (ScannedActor)
	{
		CurrentInteractionObjectComp = ScannedActor->GetComponentByClass<USTInteractionObjectComponent>();
	}
	else
	{
		return;
	}
	CurrentInteractionObjectComp->StartInteraction(GetOwner(), Index);
}

void USTInteractionSubjectComponent::EndInteraction(bool bIsSuccess)
{
	if (CurrentInteractionObjectComp == nullptr)
		return;
	// Input -> 서버 RPC 필요

	EndInteraction_Server(bIsSuccess);
}

void USTInteractionSubjectComponent::EndInteraction_Server_Implementation(bool bIsSuccess)
{
	if (CurrentInteractionObjectComp == nullptr)
		return;

	CurrentInteractionObjectComp->EndInteraction(GetOwner(), bIsSuccess);
}

void USTInteractionSubjectComponent::IncrementSelectedInteraction()
{
	if (CurrentInteractionObjectComp == nullptr)
		return;

	int32 Index = CurrentInteractionObjectComp->GetCurrentSelectedIndex();	
	int32 MaxInteractionIndex = CurrentInteractionObjectComp->GetInteractionLastIndex();
	int32 NewIndex = Index + 1;
	if (NewIndex > MaxInteractionIndex)
	{
		NewIndex = 0;
	}

	CurrentInteractionObjectComp->SetCurrentSelectedIndex(NewIndex);
}

void USTInteractionSubjectComponent::DecrementSelectedInteraction()
{
	if (CurrentInteractionObjectComp == nullptr)
		return;

	int32 Index = CurrentInteractionObjectComp->GetCurrentSelectedIndex();
	int32 MaxInteractionIndex = CurrentInteractionObjectComp->GetInteractionLastIndex();
	int32 NewIndex = Index - 1;
	if (NewIndex < 0)
	{
		NewIndex = MaxInteractionIndex;
	}

	CurrentInteractionObjectComp->SetCurrentSelectedIndex(NewIndex);
}
