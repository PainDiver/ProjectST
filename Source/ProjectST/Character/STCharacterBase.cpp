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

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASTCharacterBase

ASTCharacterBase::ASTCharacterBase()
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

}

UAbilitySystemComponent* ASTCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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

void ASTCharacterBase::ProcessInput(EInputType InputType, const FInputActionInstance& InputInstance)
{
	if (ComboComponent == nullptr)
	{
		return;
	}

	ComboComponent->ProcessCombo(InputType, InputInstance);

}

// 세 Skill 시리즈는 콤보컴포넌트를 이용해서 관리할 예정
void ASTCharacterBase::ProcessWeakAttack(const FInputActionInstance& Instance)
{
	ProcessInput(EInputType::IT_WEAK_ATTACK, Instance);
}

void ASTCharacterBase::ProcessGuard(const FInputActionInstance& Instance)
{
	ProcessInput(EInputType::IT_GUARD, Instance);
}

void ASTCharacterBase::ProcessSway(const FInputActionInstance& Instance)
{
	ProcessInput(EInputType::IT_SWAY, Instance);
}

void ASTCharacterBase::InitializeDefaultSkillSet()
{
	ComboComponent->Initialize(CharacterID);
}

void ASTCharacterBase::SetComboContext_Implementation(const FComboWindowContext& NewWindow)
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->OpenComboWindow(NewWindow);
}

void ASTCharacterBase::FlushCombo_Implementation()
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->FlushCombo();
}

void ASTCharacterBase::ClearComboContext_Implementation()
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->ClearComboWindow();
}

USTComboManagingComponent* ASTCharacterBase::GetComboComponent() const
{
	return ComboComponent;
}
