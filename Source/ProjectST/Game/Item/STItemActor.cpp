// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/STItemActor.h"
#include "Net/UnrealNetwork.h"


// Sets default values
ASTItemActor::ASTItemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void ASTItemActor::Initialize(const FReplicatedItemData& NewItemData)
{
	ItemData = NewItemData;
	if (ItemData.GetItemInfo().ItemMesh)
	{
		UStaticMesh* Mesh = ItemData.GetItemInfo().ItemMesh.LoadSynchronous();
		ItemMesh->SetStaticMesh(Mesh);
	}
}



void ASTItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTItemActor, ItemData);
}
