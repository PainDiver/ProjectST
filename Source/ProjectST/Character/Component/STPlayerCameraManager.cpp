// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STPlayerCameraManager.h"
#include "Character/STPlayerCharacter.h"
#include "Misc/STEventManager.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"


UWorld* UManagedCameraProperty::GetWorld() const
{
	UWorld* World = nullptr;
#if WITH_EDITOR
	World = GEngine->GetCurrentPlayWorld();
#endif
	return World ? World : Super::GetWorld();
}


ASTPlayerCameraManager::ASTPlayerCameraManager()
	:APlayerCameraManager()
{
	ManagedPropertyClass_Blueprint.Reserve(static_cast<uint8>(ECameraProperty::Max));
	ManagedProperties.Reserve(static_cast<uint8>(ECameraProperty::Max));

	PrimaryActorTick.bCanEverTick = true;
}

void ASTPlayerCameraManager::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(Owner);
	if (PC == nullptr)
	{
		SetActorTickEnabled(false);
		return;
	}

	ASTPlayerCharacter* Character = Cast<ASTPlayerCharacter>(PC->GetPawn());
	if (Character == nullptr)
	{
		SetActorTickEnabled(false);
		return;
	}

	OwnerCharacter = Character;
	Camera = Character->GetFollowCamera();
	SpringArm = Character->GetCameraBoom();

	if (!Character->IsLocallyControlled())
	{
		SetActorTickEnabled(false);
		return;
	}


	FSTEventDelegate Delegate;
	Delegate.BindDynamic(this, &ASTPlayerCameraManager::Initialize);
	USTEventManager::GetEventManager()->RegisterEvent_Subject(OwnerCharacter, Event_CharacterPrepared, MoveTemp(Delegate), nullptr, true);
}

void ASTPlayerCameraManager::Initialize(UObject* Data)
{
	FOnGameplayEffectTagCountChanged& Delegate = OwnerCharacter->GetAbilitySystemComponent()->RegisterGenericGameplayTagEvent();
	Delegate.Add(FOnGameplayEffectTagCountChanged::FDelegate::CreateUObject(this, &ASTPlayerCameraManager::OnStateChanged));

	for (TSubclassOf<UManagedCameraProperty> Class : ManagedPropertyClass_Blueprint)
	{
		ManagedProperties.Add(NewObject<UManagedCameraProperty>(this, Class));
	}
}


void ASTPlayerCameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OwnerCharacter == nullptr || !OwnerCharacter->IsLocallyControlled())
	{
		return;
	}

	bool bIsManualEffectValid = CurrentCameraEffect.IsValid();
	if (bIsManualEffectValid)
	{
		UpdateManualCameraEffect(DeltaTime);
	}
	
	for (UManagedCameraProperty* Property : ManagedProperties)
	{	
		if (bIsManualEffectValid && CurrentCameraEffect.ControlledProperty.Contains(Property->PropertyType))
		{
			continue;
		}

		if (Property->PropertyType == ECameraProperty::FOV ||
			Property->PropertyType == ECameraProperty::CameraLag ||
			Property->PropertyType == ECameraProperty::TargetArmLength ||
			Property->PropertyType == ECameraProperty::AspectRatio)
		{
			float Target = Property->UpdateTarget_Float(OwnerCharacter, Camera, SpringArm, DeltaTime);
			Property->PostUpdateTarget_Float(OwnerCharacter, Camera, SpringArm, Target, DeltaTime);
		}
		else
		{
			FVector Target = Property->UpdateTarget_Vector(OwnerCharacter, Camera, SpringArm, DeltaTime);
			Property->PostUpdateTarget_Vector(OwnerCharacter, Camera, SpringArm, Target, DeltaTime);
		}		
	}
	
}

void ASTPlayerCameraManager::UpdateManualCameraEffect(float DeltaTime)
{
	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::FOV))
	{
		Camera->SetFieldOfView(FMath::FInterpTo(
			Camera->FieldOfView, 
			CurrentCameraEffect.FOV, 
			DeltaTime, 
			CurrentCameraEffect.InterpSpeed));
	}

	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::CameraLag))
	{
		SpringArm->CameraLagSpeed = FMath::FInterpTo(
			SpringArm->CameraLagSpeed, 
			CurrentCameraEffect.CameraLagSpeed,
			DeltaTime,
			CurrentCameraEffect.InterpSpeed);
	}

	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::TargetArmLength))
	{
		SpringArm->TargetArmLength = FMath::FInterpTo(
			SpringArm->TargetArmLength,
			CurrentCameraEffect.TargetArmLength,
			DeltaTime,
			CurrentCameraEffect.InterpSpeed);
	}

	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::SocketOffset))
	{
		SpringArm->SocketOffset = FMath::VInterpTo(
			SpringArm->SocketOffset,
			CurrentCameraEffect.SocketOffset,
			DeltaTime,
			CurrentCameraEffect.InterpSpeed);
	}

	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::TargetOffset))
	{
		SpringArm->TargetOffset = FMath::VInterpTo(
			SpringArm->TargetOffset,
			CurrentCameraEffect.TargetOffset,
			DeltaTime,
			CurrentCameraEffect.InterpSpeed);
	}

	if (CurrentCameraEffect.ControlledProperty.Contains(ECameraProperty::AspectRatio))
	{
		Camera->bConstrainAspectRatio = true;
		Camera->AspectRatio = FMath::FInterpTo(
			Camera->AspectRatio,
			CurrentCameraEffect.AspectRatio,
			DeltaTime,
			CurrentCameraEffect.InterpSpeed);
	}

	CurrentCameraEffect.RemainingTime -= DeltaTime;
}

void ASTPlayerCameraManager::SetManualCameraEffect(const FManualCameraEffect& Effect)
{
	CurrentCameraEffect = Effect;

	if (CurrentCameraEffect.IsFOVValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::FOV);
	}

	if (CurrentCameraEffect.IsCameraLagSpeedValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::CameraLag);
	}

	if (CurrentCameraEffect.IsTargetArmLengthValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::TargetArmLength);
	}

	if (CurrentCameraEffect.IsSocketOffsetValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::SocketOffset);
	}

	if (CurrentCameraEffect.IsTargetOffsetValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::TargetOffset);
	}

	if (CurrentCameraEffect.IsAspectRatioValid())
	{
		CurrentCameraEffect.ControlledProperty.Add(ECameraProperty::AspectRatio);
	}
}

void ASTPlayerCameraManager::OnStateChanged(const FGameplayTag Tag, int32 Count)
{
	FGameplayTagContainer States = ISTStateInterface::Execute_GetStates(OwnerCharacter);
	for (UManagedCameraProperty* Property : ManagedProperties)
	{
		Property->States = States;
	}
}

