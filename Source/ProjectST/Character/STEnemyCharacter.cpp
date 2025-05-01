// Fill out your copyright notice in the Description page of Project Settings.


#include "STEnemyCharacter.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "Character/Component/STInventoryComponent.h"
#include "Misc/STEventManager.h"
#include "AI/STStateTreeComponent.h"
#include "AI/STAIController.h"

ASTEnemyCharacter::ASTEnemyCharacter(const FObjectInitializer& OI)
	:ASTCharacterBase(OI)
{
	AbilitySystemComponent = CreateDefaultSubobject<USTAbilitySystemComponent>(TEXT("ASC"));
	InventoryComponent = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComponent"));
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* ASTEnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ASTEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	ASC_Pointer = AbilitySystemComponent;

	// AI의 경우, 오너 아바타 같게, 클라,서버 처리 딱히 x
	AbilitySystemComponent->Initialize(this, this, CharacterID,
		[this]()
		{
			InitializeDefaultSkillSet();
		}
	);
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(
		[this]() 
		{
			USTEventManager::GetEventManager()->FireEvent_Subject(this, Event_CharacterPrepared, nullptr);
		}
	);

}

ASTAIController* ASTEnemyCharacter::GetSTAIController() const
{	
	return Cast<ASTAIController>(GetController());
}


