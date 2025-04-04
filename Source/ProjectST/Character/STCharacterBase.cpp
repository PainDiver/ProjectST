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
#include "Game/STGameStateInterface.h"
#include "GameFramework/GameState.h"

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

void ASTCharacterBase::BeginDestroy()
{
	Super::BeginDestroy();
}

UAbilitySystemComponent* ASTCharacterBase::GetAbilitySystemComponent() const
{
	return ASC_Pointer;
}

void ASTCharacterBase::PointAbilitySystemComponent(USTAbilitySystemComponent* ActualASC)
{
	// 베이스에있는 컴포넌트의 CreateDefaultSubobject를 하위클래스에서 할 시
	// 맵에 배치되었을때 컴포넌트가 누락되는현상 있음
	// 베이스에서는 이걸가리키는 포인터를 들고있으면 캐스팅안해도되는 이점을 누려서 만듬
	ASC_Pointer = ActualASC;
}

void ASTCharacterBase::OnPreparedBothSide(UObject* Data)
{
	if (ASC_Pointer == nullptr)
		return;

	FOnGameplayAttributeValueChange& HealthDelegate = ASC_Pointer->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
	HealthDelegate.AddLambda( 
		[this](const FOnAttributeChangeData& Data) {
			OnHealthChanged(Data);
		});
	FOnGameplayAttributeValueChange& StaminaDelegate = ASC_Pointer->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
	StaminaDelegate.AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnStaminaChanged(Data);
		});

	ASC_Pointer->AbilityCommittedCallbacks.Add(FGenericAbilityDelegate::FDelegate::CreateUObject(this,&ThisClass::OnAbilityCommitted));
	ASC_Pointer->AbilityFailedCallbacks.Add(FAbilityFailedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitFailed));
}

void ASTCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{	
	if (Data.OldValue == Data.NewValue)
		return;

	if (Data.NewValue <= 0.f)
	{
		ISTStateInterface::Execute_OnDead(this, Data.GEModData ? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr);
		
		if (GetWorld())
		{
			ISTGameStateInterface::Execute_OnCharacterDead(GetWorld()->GetGameState(),
				this, 
				Data.GEModData ? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr);
		}
	}
}

void ASTCharacterBase::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

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
	if (HasState_Implementation(CombatState_LockOn))
	{
		return;
	}

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

void ASTCharacterBase::ProcessLockOn(const FInputActionInstance& Instance)
{
	if (ISTStateInterface::Execute_HasState(this,CombatState_LockOn))
	{
		ISTStateInterface::Execute_RemoveState(this,CombatState_LockOn);
		StateHandlingComponent->RemoveManagedState(CombatState_LockOn);
	}
	else
	{
		ISTStateInterface::Execute_AddState(this,CombatState_LockOn);
		StateHandlingComponent->AddManagedState(CombatState_LockOn);
	}
	//OnProcessLockOn(Instance);
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
	if (ASC_Pointer)
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			ASC_Pointer->AddState(Tag);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer)
	{
		ASC_Pointer->RemoveState(Tag);
	}
}

bool ASTCharacterBase::AddState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer && HasAuthority())
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			ASC_Pointer->AddState_Replication(Tag);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer && HasAuthority())
	{
		ASC_Pointer->RemoveState_Replication(Tag);
	}
}

void ASTCharacterBase::OnDead_Implementation(AActor* Killer)
{
}

void ASTCharacterBase::OnKill_Implementation(AActor* Killed)
{
}


FGameplayTagContainer ASTCharacterBase::GetStates_Implementation()
{
	if (ASC_Pointer)
	{
		return ASC_Pointer->GetStates();
	}

	return FGameplayTagContainer::EmptyContainer;
}

bool ASTCharacterBase::IsImmortalState_Implementation()
{
	if (ASC_Pointer->HasState(GE_Buff_Immortal))
	{
		return true;
	}
	
	if (ASC_Pointer->HasState(State_Dead))
	{
		return true;
	}

	return false;
}

bool ASTCharacterBase::HasState_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer)
	{
		return ASC_Pointer->HasState(Tag);
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

