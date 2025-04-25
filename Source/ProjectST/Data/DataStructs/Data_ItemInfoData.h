// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/STEnum.h"
#include "GameplayEffect.h"
#include "Data_ItemInfoData.generated.h"

USTRUCT(BlueprintType)
struct FItemUseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayAbility> UseAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<UGameplayEffect> UseEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* Animation;
};


USTRUCT(BlueprintType)
struct PROJECTST_API FItemInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Desc;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FIntPoint ItemSize = FIntPoint(1,1);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EItemUseType ItemUseType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UStaticMesh> ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> ItemTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FItemUseData UseData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 EquipInfoID = 0;
};


USTRUCT(BlueprintType)
struct PROJECTST_API FItemEquipInfoData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<AActor> EquipActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName Desc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EEquipSlotType ItemSlotType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SocketName;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 StatIndex;
};