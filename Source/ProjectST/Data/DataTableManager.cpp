// Fill out your copyright notice in the Description page of Project Settings.


#include "DataTableManager.h"
#include "Misc/STDeveloperSettings.h"


#define GET_JSON_MAP(Key,DataType,JsonFile,TargetMap,KeyVarName)	\
	GetJsonDataMap<Key,DataType>(JsonFile, TargetMap, [](const DataType& Data) {return Data.KeyVarName; });


void UDataTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);	
	LoadUDataTable();
	GET_JSON_MAP(uint32, FCharacterBaseStat, "CharacterBaseStat.json", CharacterBaseStatData, ID);

}

void UDataTableManager::LoadUDataTable()
{
	const USTDeveloperSettings* Settings = GetDefault<USTDeveloperSettings>();
	if (Settings == nullptr)
	{
		return;
	}

	RootSkillSetTable = Settings->Table_RootSkillSets.LoadSynchronous();
	ItemInfoDataTable = Settings->Table_ItemData.LoadSynchronous();
	EquipItemDataTable = Settings->Table_EquipItemData.LoadSynchronous();
}

bool UDataTableManager::GetCharacterStat(int32 CharacterID, FCharacterBaseStat& OutCharacterBaseStat) const
{
	if (const FCharacterBaseStat* Stat = CharacterBaseStatData.Find(CharacterID))
	{
		OutCharacterBaseStat = *Stat;
		return true;		
	}

	return false;
}


