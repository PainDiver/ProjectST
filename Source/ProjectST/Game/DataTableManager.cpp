// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/DataTableManager.h"
#include "STDeveloperSettings.h"

#define GET_JSON_MAP(Key,DataType,JsonFile,TargetMap,KeyVarName)	\
	GetJsonDataMap<Key,DataType>(JsonFile, TargetMap, [](const DataType& Data) {return Data.KeyVarName; });

#define GET_UDATATABLE_MAP(Table,Key,DataType,TargetMap,KeyVarName) \
	UDataTable* TablePtr = Table.Get();\
	if(TablePtr == nullptr)\
	{\
		TablePtr = Table.LoadSynchronous();\
	}\
	for (const FName& Row : TablePtr->GetRowNames())\
	{\
		if (DataType* Elem = TablePtr->FindRow<DataType>(Row, "TableInit"))\
		{\
			TargetMap.Add(Elem->KeyVarName, *Elem); \
		}\
	}\
	TargetMap.Shrink();\

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
//	GET_UDATATABLE_MAP(Settings,RegisteredSkills,FGameplayTag, FSkillData, SkillData, Tag);

	GET_UDATATABLE_MAP(Settings->RegisteredRootSkillSets, int32, FRootSkillSet, RootSkillSetData, ID);
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

bool UDataTableManager::GetRootSkillSet(int32 CharacterID, FRootSkillSet& OutRootSkillSet) const
{
	if (const FRootSkillSet* SkillSet = RootSkillSetData.Find(CharacterID))
	{
		OutRootSkillSet = *SkillSet;
		return true;
	}

	return false;

}

//
//bool UDataTableManager::GetSkill(const FGameplayTag& Tag, FSkillData& OutCharacterBaseStat) const
//{
//	if (const FSkillData* Stat = SkillData.Find(Tag))
//	{
//		OutCharacterBaseStat = *Stat;
//		return true;
//	}
//
//	return false;
//}
