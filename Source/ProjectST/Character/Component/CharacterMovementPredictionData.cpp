// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterMovementPredictionData.h"
#include "GameFramework/Character.h"


void FSTCharacterNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	FCharacterNetworkMoveData::ClientFillNetworkMoveData(ClientMove, MoveType);
	
	if(ClientMove.CharacterOwner)
		LastInputVector = ClientMove.CharacterOwner->GetLastMovementInputVector();
}

bool FSTCharacterNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	bool Res1 = FCharacterNetworkMoveData::Serialize(CharacterMovement,Ar, PackageMap, MoveType);
	
	enum
	{
		LastInputVectorFlag = 1 << 0
	};

	uint8 SerializeFlag = 0;
	if (Ar.IsSaving())
	{
		if (!LastInputVector.IsZero())
		{
			SerializeFlag |= LastInputVectorFlag;
		}
	}
	
	Ar << SerializeFlag;
	if (SerializeFlag & LastInputVectorFlag)
	{
		Ar << LastInputVector;
	}
	else
	{
		LastInputVector = FVector::ZeroVector;
	}

	return Res1 && !Ar.IsError();
}
