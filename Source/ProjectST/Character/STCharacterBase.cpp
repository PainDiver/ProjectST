// Copyright Epic Games, Inc. All Rights Reserved.

#include "STCharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "GAS/STAbilitySystemComponent.h"
#include "Component/Combo/STComboManagingComponent.h"
#include "GAS/GA/STGameplayAbility.h"
#include "Character/Game/STPlayerState.h"
#include "Character/Component/STInventoryComponent.h"
#include "Component/STCharacterMovementComponent.h"
#include "Character/Component/STMotionWarpingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Game/STNativeGameplayTag.h"
#include "Character/Component/STStateHandlingComponent.h"
#include "GAS/STAttributeSet.h"
#include "Misc/STEventManager.h"
#include "GameplayEffectExtension.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASTCharacterBase

ASTCharacterBase::ASTCharacterBase(const FObjectInitializer& OI)
: Super(OI.SetDefaultSubobjectClass<USTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	ComboComponent = CreateDefaultSubobject<USTComboManagingComponent>(TEXT("ComboComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<USTMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	StateHandlingComponent = CreateDefaultSubobject<USTStateHandlingComponent>(TEXT("StateHandlingComponent"));
}

void ASTCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	FSTEventDelegate Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnPreparedBothSide);
	USTEventManager::GetEventManager()->RegisterEvent_Subject(this, Event_CharacterPrepared,Delegate ,nullptr,true);
}

UAbilitySystemComponent* ASTCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASTCharacterBase::OnPreparedBothSide(UObject* Data)
{
	FOnGameplayAttributeValueChange& HealthDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
	HealthDelegate.AddLambda( 
		[this](const FOnAttributeChangeData& Data) {
			OnHealthChanged(Data);
		});
	FOnGameplayAttributeValueChange& StaminaDelegate = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
	StaminaDelegate.AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnStaminaChanged(Data);
		});

	AbilitySystemComponent->AbilityCommittedCallbacks.Add(FGenericAbilityDelegate::FDelegate::CreateUObject(this,&ThisClass::OnAbilityCommitted));

}

void ASTCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{	
	K2_OnHealthChanged(Data.OldValue, Data.NewValue, Data.GEModData? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr);
}

void ASTCharacterBase::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	K2_OnStaminaChanged(Data.OldValue, Data.NewValue);
}


//////////////////////////////////////////////////////////////////////////
// Input



void ASTCharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASTCharacterBase::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

bool ASTCharacterBase::CanJumpInternal_Implementation() const
{	
	return Super::CanJumpInternal_Implementation() && !IsPlayingRootMotion();
}

void ASTCharacterBase::ProcessInput(ESTInputType InputType, const FInputActionInstance& InputInstance,TFunction<void(bool)>&& CallBack)
{
	if (ComboComponent == nullptr)
	{
		return;
	}

	ComboComponent->ProcessCombo(InputType, InputInstance, MoveTemp(CallBack));

}

// 세 Skill 시리즈는 콤보컴포넌트를 이용해서 관리할 예정
void ASTCharacterBase::ProcessWeakAttack(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_WEAK_ATTACK, Instance);
}

void ASTCharacterBase::ProcessGuard(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_GUARD, Instance);
}

void ASTCharacterBase::ProcessSway(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SWAY, Instance);
}

void ASTCharacterBase::ProcessSprint(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SPRINT, Instance);
}

void ASTCharacterBase::ProcessSkillQ(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SkillQ, Instance);
}

void ASTCharacterBase::ProcessSkillE(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SkillE, Instance);
}

void ASTCharacterBase::InitializeDefaultSkillSet()
{
	ComboComponent->Initialize(CharacterID);
}

void ASTCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTCharacterBase, CurrentWeaponType);
}

TMap<ESTInputType, TSubclassOf<UGameplayAbility>> ASTCharacterBase::GetRootComboSet_Implementation(EComboContextState State) const
{
	return ComboComponent->GetRootComboSet(State);
}

void ASTCharacterBase::SetComboContext(const FComboWindowContext& NewWindow)
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->OpenComboWindow(NewWindow);
}

bool ASTCharacterBase::FlushCombo(const FGameplayTagContainer& AllowedTag)
{
	if (ComboComponent == nullptr)
		return false;

	return ComboComponent->FlushCombo(AllowedTag);
}

void ASTCharacterBase::ClearComboContext()
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->ClearComboWindow();
}


bool ASTCharacterBase::AddState_Implementation(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent)
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			AbilitySystemComponent->AddState(Tag);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Implementation(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveState(Tag);
	}
}

bool ASTCharacterBase::AddState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent && HasAuthority())
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			AbilitySystemComponent->AddState_Replication(Tag);
			FOnGameplayEffectTagCountChanged&  Delegate = AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::AnyCountChange);
			Delegate.Broadcast(Tag, 1);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent && HasAuthority())
	{
		AbilitySystemComponent->RemoveState_Replication(Tag);
		FOnGameplayEffectTagCountChanged& Delegate = AbilitySystemComponent->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::AnyCountChange);
		Delegate.Broadcast(Tag, 0);
	}
}

void ASTCharacterBase::OnDead_Implementation(AActor* Killer)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ASTCharacterBase::OnKill_Implementation(AActor* Killed)
{
}

void ASTCharacterBase::BroadCastDead_Multicast_Implementation(AActor* Killer)
{
	ISTStateInterface::Execute_OnDead(this, Killer);
}

void ASTCharacterBase::BroadCastKill_Multicast_Implementation(AActor* Killed)
{
	ISTStateInterface::Execute_OnKill(this, Killed);
}

FGameplayTagContainer ASTCharacterBase::GetStates_Implementation()
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->GetStates();
	}

	return FGameplayTagContainer::EmptyContainer;
}

bool ASTCharacterBase::IsImmortalState_Implementation()
{
	if (AbilitySystemComponent->HasState(GE_Buff_Immortal))
	{
		return true;
	}
	
	if (AbilitySystemComponent->HasState(State_Dead))
	{
		return true;
	}

	return false;
}

bool ASTCharacterBase::HasState_Implementation(const FGameplayTag& Tag)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->HasState(Tag);
	}

	return false;
}

void ASTCharacterBase::OnAttributeChanged_Implementation(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	
}


USTComboManagingComponent* ASTCharacterBase::GetComboComponent() const
{
	return ComboComponent;
}

USkeletalMeshComponent* ASTCharacterBase::GetMeshComponent() const
{
	return GetMesh();
}

