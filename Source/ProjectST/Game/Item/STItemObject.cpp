// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/Item/STItemObject.h"
#include "Game/DataTableManager.h"

void USTItemObject::Initialize(int32 ID)
{
	if (UDataTableManager* TableManager = UDataTableManager::GetDataTableManager(this))
	{
		if (TableManager->GetItemInfoData(ID, ItemInfo) == false)
		{
			UE_LOG(LogTemp, Warning, TEXT("Item Not Initialized "));
		}
	}
}
