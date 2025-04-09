// Fill out your copyright notice in the Description page of Project Settings.


#include "STAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Character/STCharacterBase.h"
#include "Character/Component/STCharacterMovementComponent.h"
#include "Game/STNativeGameplayTag.h"
#include "Kismet/KismetSystemLibrary.h"

const uint8 TargetShapeCacheSize = 3;
void USTAnimInstance::NativeBeginPlay()
{
	UAnimInstance::NativeBeginPlay();
	OnMontageStarted.AddDynamic(this, &USTAnimInstance::OnMontageStared);
	OnMontageBlendingOut.AddDynamic(this, &USTAnimInstance::OnMontageEnd);
	
	InitializeCachedShapes();
	SetInitialVars();
}

void USTAnimInstance::SetInitialVars()
{
	OwnerCharacter = Cast<ASTCharacterBase>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		MovementComp = Cast<USTCharacterMovementComponent>(OwnerCharacter->GetCharacterMovement());
		bIsDedicateServer = UKismetSystemLibrary::IsDedicatedServer(OwnerCharacter);
	}
	OwingComponent = GetOwningComponent();
}


void USTAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (bIsDedicateServer == true)
		return;

	if (OwnerCharacter == nullptr)
	{
		SetInitialVars();
		return;
	}
	FVector CurrentLocation = OwnerCharacter->GetActorLocation();
	FVector CurrentForwardVector = OwnerCharacter->GetActorForwardVector();

	DisplacementSinceLastUpdate = (CurrentLocation - LastWorldLocation).Size2D();

	float Angle = FMath::RadiansToDegrees(FMath::Acos(LastForwardVector.CosineAngle2D(CurrentForwardVector)));
	YawDiffSinceLastUpdate = Angle;	


	FVector CurrentVelocity = OwnerCharacter->GetVelocity();

	Speed = CurrentVelocity.Size();
	WeaponType = OwnerCharacter->GetWeaponType();
	bIsFalling = MovementComp->IsFalling();
	

	FVector Forward = OwnerCharacter->GetActorForwardVector();	
	FVector LeftFootVector = OwingComponent->GetSocketLocation("foot_l") - OwingComponent->GetSocketLocation("pelvis");
	FVector RightFootVector = OwingComponent->GetSocketLocation("foot_r") - OwingComponent->GetSocketLocation("pelvis");
	float DotLeft = FVector::DotProduct(Forward, LeftFootVector);
	float DotRight = FVector::DotProduct(Forward, RightFootVector);
	bIsLeftFootAhead = DotLeft > DotRight;


	if (OwnerCharacter->IsPlayerControlled())
	{
		bIsMoving = MovementComp->GetLastInputVector_Rep() != FVector::ZeroVector && Speed > 0.f;
	}
	else
	{
		bIsMoving = Speed > 0.f;
	}

	bIsGuarding = ISTStateInterface::Execute_HasState(OwnerCharacter,CombatState_Guard);
	bIsSprinting = ISTStateInterface::Execute_HasState(OwnerCharacter, CombatState_Sprint);

	Acceleration_World = (CurrentVelocity - Velocity_World)/DeltaSeconds;

	FRotator Rotation = OwnerCharacter->GetActorRotation();

	if (OwnerCharacter->IsPlayerControlled())
	{
		Direction = CalculateDirection(MovementComp->GetLastInputVector_Rep(), Rotation);
	}
	else
	{
		Direction = CalculateDirection(CurrentVelocity, Rotation);
	}

	Velocity_Local = OwnerCharacter->GetActorRotation().UnrotateVector(Velocity_World);

	Speed_Ratio = Speed / MovementComp->MaxWalkSpeed;

	Velocity_Ratio = FVector2D(Velocity_Local / MovementComp->MaxWalkSpeed);

	FVector Ratio = (Acceleration_World / MovementComp->MaxAcceleration);
	Acceleration_Local_Ratio = OwnerCharacter->GetActorRotation().UnrotateVector(Ratio);
	
	TiltRatio = FMath::VInterpTo(
		TiltRatio,
		Acceleration_Local_Ratio,
		DeltaSeconds,
		Acceleration_Local_Ratio_InterpSpeed);

	Velocity_World = CurrentVelocity;

	LastWorldLocation = CurrentLocation;

	LastForwardVector = CurrentForwardVector;

	if (ShouldUpdateTurnValue())
	{
		UpdateTurnValue(DeltaSeconds);
	}
	else
	{
		ResetTurn();
	}
}

void USTAnimInstance::UpdateTurnValue(float DeltaSeconds)
{
	if (OwnerCharacter == nullptr)
		return;

	if (bIsTurning)
	{
		TurnValue = FMath::FInterpTo(TurnValue, 0.f, DeltaSeconds, Turn_InterpSpeed);
		if (abs(TurnValue) < 1.f)
		{
			ResetTurn();
		}
	}
	else if (bIsTurnLocked)
	{		
		FVector CurrentLookVector = OwnerCharacter->GetControlRotation().Vector().GetSafeNormal2D();		
		float Theta = FMath::Acos(CurrentLookVector.DotProduct(CurrentLookVector, LockedTurnValue));
		float Angle = FMath::RadiansToDegrees(Theta);

		FVector Crossed = FVector::CrossProduct(LockedTurnValue, CurrentLookVector);
		if (Crossed.Z < 0.f)
		{
			Angle = -Angle;
		}
		TurnValue = Angle;

		if ( abs(Angle) > TurnAngle)
		{
			bIsTurning = true;
		}
	}
	else
	{				
		LockedTurnValue = OwnerCharacter->GetControlRotation().Vector().GetSafeNormal2D();
		bIsTurnLocked = true;
	}
}

bool USTAnimInstance::ShouldUpdateTurnValue_Implementation()
{
	return bIsGuarding;
}

void USTAnimInstance::ResetTurn()
{
	bIsTurnLocked = false;
	TurnValue = 0.f;
	bIsTurning = false;
}

void USTAnimInstance::InitializeCachedShapes()
{
	for (int i = 0; i < TargetShapeCacheSize; i++)
	{
		UBoxComponent* Box = NewObject<UBoxComponent>(this);
		USphereComponent* Sphere = NewObject<USphereComponent>(this);
		UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this);

		Box->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
		Sphere->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
		Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Box->RegisterComponent();
		Sphere->RegisterComponent();
		Capsule->RegisterComponent();

		BoxTargetCollision.Add(Box);
		SphereTargetCollision.Add(Sphere);
		CapsuleTargetCollision.Add(Capsule);
	}
}

bool USTAnimInstance::ShouldTriggerAnimNotifyState(const UAnimNotifyState* AnimNotifyState) const
{
	// Queue에서만 호출되서 Branching타입인경우 커스터마이징해야함
	if (const UANS_Base* NotifyState = Cast<UANS_Base>(AnimNotifyState))
	{
		ENetRole Role = TryGetPawnOwner()->GetLocalRole();
		ENetMode NetMode = TryGetPawnOwner()->GetNetMode();
		bool bIsLocallyControlled = TryGetPawnOwner()->IsLocallyControlled();
		if (NetMode != NM_Standalone)
		{
			switch (NotifyState->GetNotifyRealm())
			{
				case ENotifyRealm::AuthorityOnly:
				{
					if (Role != ENetRole::ROLE_Authority)
					{
						return false;
					}
					break;
				}
				case ENotifyRealm::LocalOwnerOnly:
				{
					if (!bIsLocallyControlled)
					{
						return false;
					}
					break;
				}
				case ENotifyRealm::ExceptDedicateServer:
				{
					if (NetMode == ENetMode::NM_DedicatedServer)
					{
						return false;
					}
					break;
				}
				case ENotifyRealm::ExceptRemote:
				{
					if (NetMode == ENetMode::NM_ListenServer)
					{
						if (!bIsLocallyControlled && Role != ENetRole::ROLE_Authority)
						{
							return false;
						}
					}
					else if (NetMode == ENetMode::NM_Client)
					{
						if (!bIsLocallyControlled)
						{
							return false;
						}						
					}
					break;
				}
			}
		}

	}

	return Super::ShouldTriggerAnimNotifyState(AnimNotifyState);
}

void USTAnimInstance::OnMontageStared(UAnimMontage* Montage)
{
	MovementComp->bCanWalkOffLedges = false;
}

void USTAnimInstance::OnMontageEnd(UAnimMontage* Montage, bool bInterrupted)
{
	//Anim Notify End 이 시점 실행안됨 ..
	MovementComp->bCanWalkOffLedges = true;

	TArray<int32> IndicesToRemove;
	for (auto& Pair : CachedScratchPad)
	{
		if (Pair.Value && Pair.Value->bMarkDead)
		{
			IndicesToRemove.Add(Pair.Key);
		}
	}

	for (int32 Index : IndicesToRemove)
	{
		CachedScratchPad.Remove(Index);
	}
}

void USTAnimInstance::CacheScratchPad(int32 Key, UANS_ScratchPad* ScratchPad)
{
	if (CachedScratchPad.Contains(Key))
	{
		if (CachedScratchPad[Key] && CachedScratchPad[Key]->bMarkDead)
		{
			UE_LOG(LogTemp, Warning, TEXT("ScratchPad Collapsed and will be dead soon"));
		}
		UE_LOG(LogTemp, Warning, TEXT("ScratchPad Collapse"));
	}
	CachedScratchPad.Add(Key, ScratchPad);
}

void USTAnimInstance::RemoveScratchPad(int32 Key)
{
	CachedScratchPad.Remove(Key);
}

UANS_ScratchPad* USTAnimInstance::GetCachedScratchPad(int32 Key)
{
	if (CachedScratchPad.Contains(Key))
	{
		return CachedScratchPad[Key];
	}
	return nullptr;
}

UShapeComponent* USTAnimInstance::GetCachedShape(ETargetQueryType ShapeType)
{
	switch (ShapeType)
	{
		case ETargetQueryType::Box:
		{
			for (UBoxComponent* Box : BoxTargetCollision)
			{
				if (Box->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Box;
				}
			}
			return nullptr;
		}
		case ETargetQueryType::Sphere:
		{
			for (USphereComponent* Sphere : SphereTargetCollision)
			{
				if (Sphere->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Sphere;
				}
			}
			return nullptr;
		}
		case ETargetQueryType::Capsule:
		{
			for (UCapsuleComponent* Capsule : CapsuleTargetCollision)
			{
				if (Capsule->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Capsule;
				}
			}
			return nullptr;
		}
	}

	return nullptr;
}
