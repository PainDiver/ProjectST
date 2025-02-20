// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "STItemActor.generated.h"

class USTItemObject;

UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API ASTItemActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASTItemActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Initialize(USTItemObject* ItemObject);

private:
	// 아이템 자체의 정보를 설명하는것은 정적인데이터를 담는 USTItemObject를 이용
	UPROPERTY()
	USTItemObject* ItemBase;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess ="true"))
	UStaticMeshComponent* ItemMesh;
};
