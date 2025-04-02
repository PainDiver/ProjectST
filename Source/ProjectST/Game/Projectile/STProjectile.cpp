// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Projectile/STProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASTProjectile::ASTProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ProjectileMovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASTProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASTProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

