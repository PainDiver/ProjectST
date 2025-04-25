// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STAssetManager.h"
#include "UObject/SoftObjectPtr.h"

void USTAssetManager::LoadAsyncObject(TSoftObjectPtr<UObject> ObjectPtr, FOnAsyncLoadFinished CallBack,UObject* AdditionalData)
{
	const FSoftObjectPath& SoftObjectPath = ObjectPtr.ToSoftObjectPath();
	LoadAsync_Internal(SoftObjectPath, MoveTemp(CallBack), AdditionalData);
}

void USTAssetManager::LoadAsyncClass(TSoftClassPtr<UObject> ClassPtr, FOnAsyncLoadFinished CallBack, UObject* AdditionalData)
{
	const FSoftObjectPath& SoftObjectPath = ClassPtr.ToSoftObjectPath();
	LoadAsync_Internal(SoftObjectPath, MoveTemp(CallBack), AdditionalData);
}

void USTAssetManager::LoadAsync_Internal(const FSoftObjectPath& SoftObjectPath, FOnAsyncLoadFinished&& CallBack, UObject* AdditionalData)
{
	FStreamableManager& StreamManager = GetStreamableManager();
	FStreamableDelegate Delegate;
	if (CallBack.IsBound())
	{
		Delegate.BindLambda([MovedCallBack = MoveTemp(CallBack), AdditionalData, SoftObjectPath]()
			{
				MovedCallBack.Execute(SoftObjectPath.ResolveObject(), AdditionalData);
			});
	}
	StreamManager.RequestAsyncLoad(SoftObjectPath, Delegate);
}

bool USTAssetManager::AreAllAsyncLoadComplete()
{
	FStreamableManager& StreamManager = GetStreamableManager();
	return StreamManager.AreAllAsyncLoadsComplete();
}

