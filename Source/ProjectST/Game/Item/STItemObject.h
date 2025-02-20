// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Game/TableRows/Data_ItemInfo.h"
#include "STItemObject.generated.h"

/**
 * 
 */
class USTPrimaryDataAsset_ItemUse;

// USTItemObject는 기본적으로 정적인데이터(메쉬같은)와 동적인데이터(개수같은)로 이루어짐
// 정적데이터의 경우 ID만으로도 테이블에서 재복원이 가능함
// 
// 동적데이터의 경우는 다르게 관리예정
// 아이템의
// 1. 아이템의 생성은 CreateItemObject()에 의해서만 실행될 예정
// 2. 생성 후 아이템에 고유 HashID가 부여됨
// 3. FastArraySerializer로 HashID, ReplicatableData 형태로 동적데이터가 저장될 예정
// 4. Replicated는 쓰되 최적화를 위해 리플리케이션 커스터마이징 필요

UCLASS(Blueprintable)
class PROJECTST_API USTItemObject : public UObject
{
	GENERATED_BODY()
	
public:

	void Initialize(int32 ID);

	const FItemInfoData& GetItemInfo() { return ItemInfo; }

private:

	UPROPERTY(Transient)
	FItemInfoData ItemInfo;

	// 
};
