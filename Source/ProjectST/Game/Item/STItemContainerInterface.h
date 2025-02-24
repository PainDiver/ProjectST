// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/STEnum.h"
#include "Game/Item/ReplicatedItemData.h"
#include "STItemContainerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTItemContainerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTST_API ISTItemContainerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual FReplicatedItemContainer* GetContainer(EItemContainerType ContainerType) = 0;

};
