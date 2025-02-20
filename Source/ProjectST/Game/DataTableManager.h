// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TableRows/Data_CharacterBaseStat.h"
#include "TableRows/Data_Skill.h"
#include "TableRows/Data_RootSkillSet.h"
#include "TableRows/Data_ItemInfo.h"

#include "Engine/AssetManager.h"

#include "JsonUtilities.h"
#include "GameplayTagContainer.h"


#include "DataTableManager.generated.h"

/**
 * 
 */
#define GET_JSON_DATA_MAP(StructType,FileName,Target,KeyVariable)	\
	GetJsonDataMap()


// UAsset과 관련없는 데이터의 경우
// JSON으로 관리
// UAsset관련 있을 시, DeveloperSetting에서 테이블 매핑 후 Initialize에서 가져오기

UCLASS()
class PROJECTST_API UDataTableManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	static UDataTableManager* GetDataTableManager(UObject* Subject)
	{
		if (Subject == nullptr)
			return nullptr;

		UGameInstance* GameInstance = Subject->GetWorld()->GetGameInstance();
		if (GameInstance == nullptr)
			return nullptr;
		
		return GameInstance->GetSubsystem<UDataTableManager>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection)override;

	// UdataTable -> TMap형태로 바꿈
	void LoadUDataTable();

	//Array로 JsonData를 받아내는함수
	template<typename DataType>
	inline bool GetJsonDataArray(const FString& JsonFileName, TArray<DataType>& OutArray);

	//Map으로 JsonData를 받아내는함수
	template<typename KeyType, typename DataType>
	bool GetJsonDataMap(const FString& JsonFileName, TMap<KeyType, DataType>& OutMap,TFunction<KeyType(const DataType&)>&& GetKeyFunction);

	UFUNCTION(BlueprintCallable)
	bool GetCharacterStat(int32 CharacterID, FCharacterBaseStat& OutCharacterBaseStat)const;

	UFUNCTION(BlueprintCallable)
	bool GetRootSkillSet(int32 CharacterID, FRootSkillSet& OutRootSkillSet)const;

	UFUNCTION(BlueprintCallable)
	bool GetItemInfoData(int32 ItemID, FItemInfoData& OutItemInfo)const;


	template<typename T, 
		typename StaticAssert = typename TEnableIf<TIsDerivedFrom<T, UPrimaryDataAsset>::Value>::Type> // Typename True
	void LoadPrimaryDataAndCallBack(const FPrimaryAssetId& AssetID, TFunction<void(T*)>&& CallBack)
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		if (UObject* Obj = AssetManager.GetPrimaryAssetObject(AssetID))
		{
			CallBack(Cast<T>(Obj));
		}
		else
		{
			// PDA 로드
			AssetManager.LoadPrimaryAsset(AssetID, TArray<FName>(),
				FStreamableDelegate::CreateLambda([](UPrimaryDataAsset* Obj)
					{
						CallBack(Cast<T>(Obj));
					}));
		}
	}


	//UFUNCTION(BlueprintCallable)
	//bool GetSkill(const FGameplayTag& Tag, FSkillData& OutCharacterBaseStat)const;

private:

	//모든 캐릭터 스탯을 들고있음, 전역에서 ID->Stat 형태로 접근
	TMap<uint32, FCharacterBaseStat> CharacterBaseStatData;

	//// UAsset과 연관지어야하기때문에 DataTable로 관리 
	//UPROPERTY()
	//TMap<FGameplayTag, FSkillData> SkillData;

	// 캐릭터 별 Default 스킬테이블을 ID로 관리
	TMap<uint32, FRootSkillSet> RootSkillSetData;

	TMap<uint32, FItemInfoData> ItemInfoData;

};

template<typename DataType>
inline bool UDataTableManager::GetJsonDataArray(const FString& JsonFileName,TArray<DataType>& OutArray)
{

	FString FilePath = FPaths::ProjectContentDir() + "Project/JsonData/" + JsonFileName;
	FString JsonString;
	if (!FFileHelper::LoadFileToString(JsonString, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load JSON file: %s"), *FilePath);
		return false;
	}

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON file"));
		return false;

	}

	// 일단 Json Lists 내부에있는 {},{}형태로 받아낼 예정임
	TArray<TSharedPtr<FJsonValue>> JsonItems = JsonObject->GetArrayField("Lists");
	for (TSharedPtr<FJsonValue> JsonItem : JsonItems)
	{
		TSharedPtr<FJsonObject> ItemObject = JsonItem->AsObject();
		if (!ItemObject.IsValid()) continue;

		DataType NewStruct;
		if (FJsonObjectConverter::JsonObjectToUStruct(ItemObject.ToSharedRef(), DataType::StaticStruct(), &NewStruct))
		{
			OutArray.Add(NewStruct);
		}
	}

	return true;
}


template<typename KeyType,typename DataType>
inline bool UDataTableManager::GetJsonDataMap(const FString& JsonFileName, TMap<KeyType, DataType>& OutMap, TFunction<KeyType(const DataType&)>&& GetKeyFunction)
{
	TArray<DataType> DataArray;
	if (GetJsonDataArray<DataType>(JsonFileName,DataArray) == false)
	{
		return false;
	}
	
	for (DataType& Element : DataArray)
	{
		OutMap.Add(GetKeyFunction(Element), Element);
	}

	return true;
}
