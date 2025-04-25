// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Misc/STEnum.h"
#include "Character/Component/STInventoryComponent.h"
#include "Character/Game/STPlayerState.h"
#include "Character/Component/STInteractionSubjectComponent.h"
#include "STCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USTCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTST_API ISTCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void SetWeaponType(EWeaponType WeaponType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EWeaponType GetWeaponType()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCurrentScannedActor()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USTInventoryComponent* GetInventoryComponent()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ASTPlayerState* GetSTPlayerState()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USTInteractionSubjectComponent* GetInteractionSubjectComponent()const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USTInteractionObjectComponent* GetInteractionObjectComponent()const;

};
