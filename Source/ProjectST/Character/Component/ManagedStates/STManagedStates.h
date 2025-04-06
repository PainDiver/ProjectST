// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "STManagedStates.generated.h"

class USTStateHandlingComponent;

UENUM(Blueprintable, BlueprintType)
enum class EManagedStateRealm : uint8
{
	All,
	Server,
	OnlyClients,
	OwnerClientOnly,
	ExceptRemote
};

UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class USTManagedState : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	bool CanAddState(AActor* StateOwner);
	virtual bool CanAddState_Implementation(AActor* StateOwner) { return true; };

	UFUNCTION(BlueprintNativeEvent)
	void OnStateAdded(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent);
	virtual void OnStateAdded_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent);

	UFUNCTION(BlueprintNativeEvent)
	void OnTick(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent,float DeltaTime);
	virtual void OnTick_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent, float DeltaTime) {};

	UFUNCTION(BlueprintNativeEvent)
	void OnStateRemoved(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent);
	virtual void OnStateRemoved_Implementation(AActor* StateOwner, USTStateHandlingComponent* OwnerComponent);

	FORCEINLINE bool IsRemoved() const { return bIsRemoved; }

	bool IsMatchingState(const FGameplayTag Tag);

	FORCEINLINE FGameplayTag& GetTag() { return StateTag; }

	void ResolveCollapsingStates(AActor* StateOwner,USTStateHandlingComponent* OwnerComponent);

	FORCEINLINE EManagedStateRealm GetRealm()const{ return Realm;}

protected:
	FGameplayTagContainer TagNotAllowed;

private:

	bool bIsRemoved = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FGameplayTag StateTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EManagedStateRealm Realm;

};
