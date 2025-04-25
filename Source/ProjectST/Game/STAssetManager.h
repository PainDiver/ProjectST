// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "STAssetManager.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnAsyncLoadFinished, UObject*, Object, UObject*, AdditionalData);

UCLASS()
class PROJECTST_API USTAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintPure)
	static USTAssetManager* GetSTAssetManager()
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		USTAssetManager* STAssetManager = Cast<USTAssetManager>(&AssetManager);
		return STAssetManager;
	}

	UFUNCTION(BlueprintCallable)
	static void LoadAsyncObject(TSoftObjectPtr<UObject> ObjectPtr, FOnAsyncLoadFinished CallBack, UObject* AdditionalData);

	UFUNCTION(BlueprintCallable)
	static void LoadAsyncClass(TSoftClassPtr<UObject> ClassPtr, FOnAsyncLoadFinished CallBack, UObject* AdditionalData);

	static void LoadAsync_Internal(const FSoftObjectPath& Path, FOnAsyncLoadFinished&& CallBack, UObject* AdditionalData);

	UFUNCTION(BlueprintPure)
	static bool AreAllAsyncLoadComplete();


private:

};
