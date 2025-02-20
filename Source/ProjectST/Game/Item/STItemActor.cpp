// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/STItemActor.h"
#include "Game/Item/STItemObject.h"

// Sets default values
ASTItemActor::ASTItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
}

// Called when the game starts or when spawned
void ASTItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASTItemActor::Initialize(USTItemObject* ItemObject)
{
	ItemBase = ItemObject;
	if (ItemBase && ItemBase->GetItemInfo().ItemMesh)
	{
		UStaticMesh* Mesh = ItemBase->GetItemInfo().ItemMesh.Get();
		if (Mesh == nullptr)
		{
			ItemBase->GetItemInfo().ItemMesh.LoadSynchronous();
			Mesh = ItemBase->GetItemInfo().ItemMesh.Get();
		}
		ItemMesh->SetStaticMesh(Mesh);
	}
}

