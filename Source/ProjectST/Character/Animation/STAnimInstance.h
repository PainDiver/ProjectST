// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AnimNotify/ANS_Base.h"
#include "STEnum_Anim.h"
#include "STAnimInstance.generated.h"


class UBoxComponent;
class USphereComponent;
class UCapsuleComponent;

UCLASS()
class PROJECTST_API USTAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeBeginPlay()override;

	void InitializeCachedShapes();


	void CacheScratchPad(const FString& Key,UANS_ScratchPad* ScratchPad);
	void RemoveScratchPad(const FString& Key);
	UANS_ScratchPad* GetCachedScratchPad(const FString& Key);
	UShapeComponent* GetCachedShape(ETargetQueryType ShapeType);

private:
	UPROPERTY()
	TArray<UBoxComponent*> BoxTargetCollision;
	UPROPERTY()
	TArray<USphereComponent*> SphereTargetCollision;
	UPROPERTY()
	TArray<UCapsuleComponent*> CapsuleTargetCollision;

	UPROPERTY()
	TMap<FString,UANS_ScratchPad*> CachedScratchPad;
};
