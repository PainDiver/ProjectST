// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/STCheatComponent.h"


// Sets default values for this component's properties
USTCheatComponent::USTCheatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	// ...
}


// Called when the game starts
void USTCheatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

bool USTCheatComponent::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	CallFunctionByNameWithArguments(Cmd, Ar, Executor, true);
	return false;
}
