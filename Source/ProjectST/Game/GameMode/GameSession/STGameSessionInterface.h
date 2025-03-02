// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/DataStructs/Data_AccountData.h"
#include "STGameSessionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTGameSessionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTST_API ISTGameSessionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	
	UFUNCTION(BlueprintNativeEvent)
	bool FinishSpawning(APlayerController* PlayerController, const FAccountData& AccountData);

};
