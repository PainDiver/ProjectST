// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STGameInstance.h"
#include "ShaderCompiler.h"


void USTGameInstance::ForceStreamingLoadAndCompile()
{
	GShaderCompilingManager->FinishAllCompilation();
	IStreamingManager::Get().BlockTillAllRequestsFinished();
}
