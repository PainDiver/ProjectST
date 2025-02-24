// Fill out your copyright notice in the Description page of Project Settings.


#include "STEnemyCharacter.h"
#include "Character/Component/Combo/STComboManagingComponent.h"
#include "Character/Component/STInventoryComponent.h"

ASTEnemyCharacter::ASTEnemyCharacter()
	:Super()
{
	AbilitySystemComponent = CreateDefaultSubobject<USTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	InventoryComponent = CreateDefaultSubobject<USTInventoryComponent>(TEXT("InventoryComponent"));
}

void ASTEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// AI의 경우, 오너 아바타 같게, 클라,서버 처리 딱히 x
	AbilitySystemComponent->Initialize(this,this, CharacterID,
		[this]()
		{
			InitializeDefaultSkillSet();
		}
	);

}


