// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"

struct FSTCharacterNetworkMoveData : public FCharacterNetworkMoveData
{
public:
	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)override;
	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)override;

	FVector_NetQuantize LastInputVector;
};

struct FSTCharacterNetworkMoveDataContainer : public FCharacterNetworkMoveDataContainer
{
public:

	FSTCharacterNetworkMoveDataContainer()
		: FCharacterNetworkMoveDataContainer()
	{
		NewMoveData = &BaseDefaultMoveData[0];
		PendingMoveData = &BaseDefaultMoveData[1];
		OldMoveData = &BaseDefaultMoveData[2];
	}
protected:

	FSTCharacterNetworkMoveData BaseDefaultMoveData[3];
};
