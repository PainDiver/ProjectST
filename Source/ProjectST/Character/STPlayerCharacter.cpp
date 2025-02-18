// Fill out your copyright notice in the Description page of Project Settings.


#include "STPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Control/STDataAsset_Input.h"
#include "GameFramework/PlayerState.h"
#include "Character/Component/Combo/STComboManagingComponent.h"

ASTPlayerCharacter::ASTPlayerCharacter()
	:Super()
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


void ASTPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//서버 Initialization
	AbilitySystemComponent = Cast<USTAbilitySystemComponent>(GetAbilitySystemComponent());

	if (AbilitySystemComponent)
	{
		//플레이어의 경우 플레이어스테이트가 오너가 됨
		AbilitySystemComponent->Initialize(GetPlayerState(),this, CharacterID,
			[this]()
			{
				// OnInit 콜백 넘김, Initialize에 종속성 안생김
				InitializeDefaultCombo();
			}
		);
	}
}

void ASTPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	//클라 Initialization
	AbilitySystemComponent = Cast<USTAbilitySystemComponent>(GetAbilitySystemComponent());

	if (AbilitySystemComponent)
	{
		//플레이어의 경우 플레이어스테이트가 오너가 됨
		AbilitySystemComponent->Initialize(GetPlayerState(), this, CharacterID,
			[this]()
			{
				InitializeDefaultCombo();
			}
		);
	}
}

UAbilitySystemComponent* ASTPlayerCharacter::GetAbilitySystemComponent() const
{
	if (AbilitySystemComponent == nullptr)
	{
		if (APlayerState* PS = GetPlayerState())
		{
			if (IAbilitySystemInterface* IASC = Cast<IAbilitySystemInterface>(PS))
			{
				return IASC->GetAbilitySystemComponent();
			}
		}
	}
	
	return AbilitySystemComponent;
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



void ASTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		for (const FInputDataBinder& Input : InputData->TagInputs)
		{
			FText FuncName = UEnum::GetDisplayValueAsText(Input.Function);
			EnhancedInputComponent->BindAction(Input.InputAction, Input.TriggerType, this, *FuncName.ToString());
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}
