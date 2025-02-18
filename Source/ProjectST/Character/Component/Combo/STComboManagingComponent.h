// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/Control/STDataAsset_Input.h"
#include "Misc/STEnum.h"
#include "STComboManagingComponent.generated.h"


class UAbilitySystemComponent;
class UGameplayAbility;
class ASTCharacterBase;
class UComboContext;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTComboManagingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTComboManagingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void ProcessCombo(EInputType InputType, const FInputActionInstance& InputInstance);

	// 각 인풋 별 어빌리티를 매핑함
	void SetRootCombo(int CharacterID);

	// 상태 판단
	EComboContextState GetComboContextState(ASTCharacterBase* Character)const;

	TMap<EInputType, TSubclassOf<UGameplayAbility>> GetRootComboSet()const { return RootComboSet; }

private:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<EInputType, TSubclassOf<UGameplayAbility>> RootComboSet;

	UPROPERTY()
	TMap<EComboContextState, UComboContext*> ComboContextMap;

	UPROPERTY()
	UAbilitySystemComponent* OwnerASC;
};
