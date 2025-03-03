// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "STNetSettings.generated.h"

/**
 * 
 */
UCLASS(Config = STNet, DefaultConfig)
class STNET_API USTNetSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	UPROPERTY(Config, EditAnywhere, Category = "Net")
	FString ServerIP;

	UPROPERTY(Config, EditAnywhere, Category = "Net")
	int32 ServerPort;
};
