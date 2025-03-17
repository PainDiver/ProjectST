// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "STGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class PROJECTST_API USTGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void ForceStreamingLoadAndCompile();
	
	virtual void Init()override;

	// 서브게임인스턴스 이니셜라이징 이후 블루프린트노출
	UFUNCTION(BlueprintImplementableEvent)
	void PostInitialize();
};
