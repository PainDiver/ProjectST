// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "TargetData.generated.h"


USTRUCT(BlueprintType)
struct FGameplayAbilityTargetData_Vector : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FGameplayAbilityTargetData_Vector::StaticStruct();
	}
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Data;
		return !Ar.IsError();
	}
	UPROPERTY()
	TArray<FVector> Data;
};
template<>
struct TStructOpsTypeTraits<FGameplayAbilityTargetData_Vector> : public TStructOpsTypeTraitsBase2<FGameplayAbilityTargetData_Vector>
{
	enum
	{
		WithNetSerializer = true,
		
	};
};

USTRUCT(BlueprintType)
struct PROJECTST_API FSTGenericGameplayAbilityTargetDataParams
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FGameplayTagContainer TagContainer;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UObject*> Objects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FVector> Vectors;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> Floats;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<uint8> Enums;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<int32> Ints;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FName> Names;

};

UCLASS(BlueprintType, Blueprintable)
class PROJECTST_API USTGenericGameplayAbilityTargetDataParamObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FSTGenericGameplayAbilityTargetDataParams Params;
};


USTRUCT()
struct PROJECTST_API FSTGenericGameplayAbilityTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSTGenericGameplayAbilityTargetData::StaticStruct();
	}
	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	void SetParam(FSTGenericGameplayAbilityTargetDataParams&& Params)
	{
		TagContainer = MoveTemp(Params.TagContainer);
		Objects = MoveTemp(Params.Objects);
		Vectors = MoveTemp(Params.Vectors);
		Floats = MoveTemp(Params.Floats);
		Enums = MoveTemp(Params.Enums);
		Ints = MoveTemp(Params.Ints);
		Names = MoveTemp(Params.Names);
	}

	UPROPERTY()
	FGameplayTagContainer TagContainer;

	UPROPERTY()
	TArray<UObject*> Objects;

	UPROPERTY()
	TArray<FVector> Vectors;

	UPROPERTY()
	TArray<float> Floats;

	UPROPERTY()
	TArray<uint8> Enums;

	UPROPERTY()
	TArray<int32> Ints;

	UPROPERTY()
	TArray<FName> Names;
};

template<>
struct TStructOpsTypeTraits<FSTGenericGameplayAbilityTargetData> : public TStructOpsTypeTraitsBase2<FSTGenericGameplayAbilityTargetData>
{
	enum
	{
		WithNetSerializer = true,
	};
};
