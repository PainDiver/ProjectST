// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/STGameBlueprintFunctionLibrary.h"
#include "Game/STGameInstance.h"
#include "Kismet/GameplayStatics.h"

bool USTGameBlueprintFunctionLibrary::IsEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

void USTGameBlueprintFunctionLibrary::DispatchAsyncTask(FAsyncTaskDelegate Delegate)
{
	AsyncTask(ENamedThreads::Type::AnyThread,
		[Task = MoveTemp(Delegate)]()
		{
			Task.ExecuteIfBound();
		});
}

USTGameInstance* USTGameBlueprintFunctionLibrary::GetSTGameInstance()
{
	if(GEngine)
		return Cast<USTGameInstance>(UGameplayStatics::GetGameInstance(GEngine->GetCurrentPlayWorld()));

	return nullptr;
}


