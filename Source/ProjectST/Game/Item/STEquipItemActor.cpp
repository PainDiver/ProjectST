// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/STEquipItemActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

ASTEquipItemActor::ASTEquipItemActor(const FObjectInitializer& OI)
	:Super()
{
	bReplicates = true;
}

void ASTEquipItemActor::Initialize_Implementation(AActor* Avatar, const FReplicatedItemData& Data, const FItemEquipInfoData& EquipData)
{
	ItemData = Data;

	UStaticMesh* Mesh = Data.ItemInfo.ItemMesh.LoadSynchronous();

	if (ItemMesh)
	{
		ItemMesh->SetStaticMesh(Mesh);
	}

	EquipInfo = EquipData;

	if (EquipData.SocketName.IsNone() == false)
	{
		FAttachmentTransformRules Rules
		{
			EAttachmentRule::SnapToTarget ,
			EAttachmentRule::SnapToTarget ,
			EAttachmentRule::SnapToTarget ,
			false
		};
		if (ACharacter* AvatarCharacter = Cast<ACharacter>(Avatar))
		{
			AttachToComponent(AvatarCharacter->GetMesh(), Rules, EquipData.SocketName);
		}
	}
}
