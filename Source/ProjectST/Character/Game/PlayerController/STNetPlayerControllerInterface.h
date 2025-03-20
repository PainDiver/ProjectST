// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "STNetPlayerControllerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTNetPlayerControllerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */

struct FAccountData;
class PROJECTST_API ISTNetPlayerControllerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void ConfirmAccountData() = 0;

	virtual TSharedPtr<FAccountData> GetAccountData() = 0;

};
