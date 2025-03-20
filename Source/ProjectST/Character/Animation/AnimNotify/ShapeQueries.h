// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShapeQueries.generated.h"


UCLASS(BlueprintType, Blueprintable)
class UQueryShape : public UObject
{
	GENERATED_BODY()
public:
	virtual void Query() {};
};
UCLASS(BlueprintType, Blueprintable)
class UBoxQueryShape : public UQueryShape
{
	GENERATED_BODY()
public:
	virtual void Query()override {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector BoxExtent;
};
UCLASS(BlueprintType, Blueprintable)
class USphereQueryShape : public UQueryShape
{
	GENERATED_BODY()
public:
	virtual void Query()override {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
};
UCLASS(BlueprintType, Blueprintable)
class UCapsuleQueryShape : public UQueryShape
{
	GENERATED_BODY()
public:
	virtual void Query()override {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HalfHeight;
};

