// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/AccountManager.h"

void UAccountManager::ChooseCharacter(int32 CharacterID)
{
	LocalAccountData.ChosenCharacterID = CharacterID;
	OnLocalAccountDataChanged.Broadcast(LocalAccountData);
}

void UAccountManager::SetLocalAccountData(const FAccountData& Data)
{
	LocalAccountData = Data;
	OnLocalAccountDataChanged.Broadcast(Data);
}
