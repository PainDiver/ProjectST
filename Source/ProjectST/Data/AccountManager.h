// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/DataStructs/Data_AccountData.h"
#include "AccountManager.generated.h"


UCLASS()
class PROJECTST_API UAccountManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	
	static UAccountManager* GetAccountManager()
	{
		if (GEngine == nullptr)
			return nullptr;

		UGameInstance* GameInstance = GEngine->GetCurrentPlayWorld()->GetGameInstance();
		if (GameInstance == nullptr)
			return nullptr;

		return GameInstance->GetSubsystem<UAccountManager>();
	}

	UFUNCTION(BlueprintCallable)
	void ChooseCharacter(int32 CharacterID);

	UFUNCTION(BlueprintCallable)
	void SetLocalAccountData(const FAccountData& Data);


	UFUNCTION(BlueprintPure)
	FORCEINLINE FAccountData GetLocalAccountData() const { return LocalAccountData; };
	
	FORCEINLINE FAccountData& GetLocalAccountData_Ref(){ return LocalAccountData; };

private:
	FAccountData LocalAccountData;


public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLocalAccountDataChanged, const FAccountData&, Data);

	UPROPERTY(VisibleAnywhere,BlueprintAssignable)
	FOnLocalAccountDataChanged OnLocalAccountDataChanged;
};

