// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "STDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = DefaultData, DefaultConfig)
class PROJECTST_API USTDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:

	// 객체를 직접저장하는것은 불가능
	UPROPERTY(EditAnywhere, config)
	TSoftObjectPtr<UDataTable> Table_RootSkillSets;

	UPROPERTY(EditAnywhere, config)
	TSoftObjectPtr<UDataTable> Table_ItemData;

	UPROPERTY(EditAnywhere, config)
	TSoftObjectPtr<UDataTable> Table_EquipItemData;
};
