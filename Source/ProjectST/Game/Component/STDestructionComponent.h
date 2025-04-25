// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STDestructionComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDesturction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDamage,int32,Damage);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTDestructionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTDestructionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	
	UFUNCTION(BlueprintCallable)
	void DamageObject(int32 BreakDamage);

	void Destruct();

	UFUNCTION()
	void OnRep_BreakHP(int32 OldBreakHP);

private:
	UPROPERTY(BlueprintReadWrite,EditAnywhere,ReplicatedUsing = OnRep_BreakHP, meta = (AllowPrivateAccess = "true"))
	int32 BreakHP = 100;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnDesturction OnDestructionDelegate;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnDamage OnDamageDelegate;
};
