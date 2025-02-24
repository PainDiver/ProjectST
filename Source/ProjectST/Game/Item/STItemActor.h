// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Game/Item/ReplicatedItemData.h"
#include "STItemActor.generated.h"

class USTItemObject;

UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTItemActor();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(const FReplicatedItemData& ItemData);

	UFUNCTION(BlueprintPure)
	FReplicatedItemData GetItemData()const{ return ItemData; }

private:

	UPROPERTY(replicated)
	FReplicatedItemData ItemData;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* ItemMesh;
};
