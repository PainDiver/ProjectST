// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Data/DataAsset/STDataAsset_Input.h"
#include "GameFramework/PlayerState.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "Misc/STEventManager.h"
#include "Character/Component/STStateHandlingComponent.h"
#include "Character/Component/ManagedStates/STManagedState_LockOn.h"
#include "Game/Component/STInteractionObjectComponent.h"


ASTPlayerCharacter::ASTPlayerCharacter(const FObjectInitializer& OI)
	:ASTCharacterBase(OI)
{
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	
}


void ASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		if (USTManagedState_LockOn* State = Cast<USTManagedState_LockOn>(StateHandlingComponent->FindState(CombatState_LockOn)))
		{
			State->OnLockOnTargetSet.AddDynamic(this, &ASTPlayerCharacter::OnSetLockOnTarget);
			State->OnLockOnTargetRemoved.AddDynamic(this, &ASTPlayerCharacter::OnRemoveLockOnTarget);
		}
	}
}

void ASTPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ASTPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//서버 Initialization
	if (GetPlayerState())
	{
		ASC_Pointer = Cast<USTAbilitySystemComponent>(GetAbilitySystemComponent());
		if (ASC_Pointer)
		{
			//플레이어의 경우 플레이어스테이트가 오너가 됨
			ASC_Pointer->Initialize(GetPlayerState(), this, CharacterID,
				[this]()
				{
					// OnInit 콜백 넘김, Initialize에 종속성 안생김
					InitializeDefaultSkillSet();
				}
			);
		}

		CheckPlayerStateReplication();
	}
}

void ASTPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//클라 Initialization
	if (GetPlayerState())
	{
		ASC_Pointer = Cast<USTAbilitySystemComponent>(GetAbilitySystemComponent());
		if (ASC_Pointer)
		{
			//플레이어의 경우 플레이어스테이트가 오너가 됨
			ASC_Pointer->Initialize(GetPlayerState(), this, CharacterID,
				[this]()
				{
					InitializeDefaultSkillSet();
				}
			);
		}
		USTEventManager::GetEventManager()->FireEvent_Subject(this, Event_CharacterPrepared, nullptr);
	}
}

UAbilitySystemComponent* ASTPlayerCharacter::GetAbilitySystemComponent() const
{
	if (ASC_Pointer == nullptr)
	{
		if (APlayerState* PS = GetPlayerState())
		{
			if (IAbilitySystemInterface* IASC = Cast<IAbilitySystemInterface>(PS))
			{
				return IASC->GetAbilitySystemComponent();
			}
		}
	}
	
	return ASC_Pointer;
}

void ASTPlayerCharacter::CheckPlayerStateReplication_Implementation()
{
	OnPlayerStateReplicationChecked();
}

void ASTPlayerCharacter::OnPlayerStateReplicationChecked_Implementation()
{
	USTEventManager::GetEventManager()->FireEvent_Subject(this, Event_CharacterPrepared, nullptr);
}


void ASTPlayerCharacter::SetLockOnTarget_Server_Implementation(AActor* Actor)
{
	LockOnTarget = Actor;
}

void ASTPlayerCharacter::RemoveLockOnTarget_Server_Implementation()
{
	LockOnTarget = nullptr;
}

void ASTPlayerCharacter::OnSetLockOnTarget(AActor* Actor)
{
	SetLockOnTarget_Server(Actor);
	LockOnTarget = Actor;
}

void ASTPlayerCharacter::OnRemoveLockOnTarget()
{
	RemoveLockOnTarget_Server();
	LockOnTarget = nullptr;
}

void ASTPlayerCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

#define BIND_INPUT(InputBinder,EnumCase,Class,FunctionName)\
	case EnumCase:\
		EnhancedInputComponent->BindAction(InputBinder.InputAction, InputBinder.TriggerType, this, &Class::FunctionName );\
		break; \

void ASTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		if(InputData)
		{ 
			for (const FInputDataBinder& Input : InputData->TagInputs)
			{
				// UFUNCTION 리플렉션으로 이름으로 했지만,.,, 이렇게되면 FInputActionInstance를 못받음
				// FuncName -> Mem FuncAddress 방식으로 런타임에 해야작동
				switch (Input.Function)
				{
					BIND_INPUT(Input, EActionFunctionType::Move, ASTCharacterBase, ProcessMove)
					BIND_INPUT(Input, EActionFunctionType::Look, ASTCharacterBase, ProcessLook)
					BIND_INPUT(Input, EActionFunctionType::Jump, ASTCharacterBase, Jump)
					BIND_INPUT(Input, EActionFunctionType::WeakAttack, ASTCharacterBase, ProcessWeakAttack)
					BIND_INPUT(Input, EActionFunctionType::Guard, ASTCharacterBase, ProcessGuard)
					BIND_INPUT(Input, EActionFunctionType::Sway, ASTCharacterBase, ProcessSway)
					BIND_INPUT(Input, EActionFunctionType::Sprint, ASTCharacterBase, ProcessSprint)
					BIND_INPUT(Input, EActionFunctionType::SkillQ, ASTCharacterBase, ProcessSkillQ)
					BIND_INPUT(Input, EActionFunctionType::SkillE, ASTCharacterBase, ProcessSkillE)
					BIND_INPUT(Input, EActionFunctionType::LockOn, ASTCharacterBase, ProcessLockOn)
					BIND_INPUT(Input, EActionFunctionType::SkillR, ASTCharacterBase, ProcessSkillR)
					BIND_INPUT(Input, EActionFunctionType::Interaction, ASTCharacterBase, ProcessInteraction)
					BIND_INPUT(Input, EActionFunctionType::SelectInteraction, ASTCharacterBase, ProcessSelectInteraction)
				default:
					break;
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
