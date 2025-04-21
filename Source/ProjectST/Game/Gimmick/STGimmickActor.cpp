// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Gimmick/STGimmickActor.h"


// Sets default values
ASTGimmickActor::ASTGimmickActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASTGimmickActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTGimmickActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

