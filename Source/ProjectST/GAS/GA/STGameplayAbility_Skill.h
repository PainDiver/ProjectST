// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STGameplayAbility.h"
#include "Character/Control/STDataAsset_Input.h"
#include "STGameplayAbility_Skill.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTST_API USTGameplayAbility_Skill : public USTGameplayAbility
{
	GENERATED_BODY()
		
public:
	
	UFUNCTION(BlueprintNativeEvent)
	TSubclassOf<UGameplayAbility> PickCombo(AActor* Character,EInputType Input, const FInputActionInstance& InputInstance);
	TSubclassOf<UGameplayAbility> PickCombo_Implementation(AActor* Character, EInputType Input, const FInputActionInstance& InputInstance);

private:


	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TMap<EInputType, TSubclassOf<UGameplayAbility>> Combos;
};
