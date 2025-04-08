// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "STParkourComponent.generated.h"

class USTStateHandlingComponent;
class UGameplayAbility;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnParkourRequested, const TArray<FVector>&, ParkourPoints);

UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class UParkourState : public UObject
{
	GENERATED_BODY()

public:

	virtual UWorld* GetWorld()const override;

	UFUNCTION(BlueprintNativeEvent)
	void OnStartPreparing(AActor* Owner);
	void OnStartPreparing_Implementation(AActor* Owner){}

	UFUNCTION(BlueprintNativeEvent)
	void OnEndPreparing(AActor* Owner);
	void OnEndPreparing_Implementation(AActor* Owner){}

	// 요거 파쿠르 체크 + 파쿠르 포인트 저장용
	UFUNCTION(BlueprintNativeEvent)
	bool PrepareParkour(AActor* StateOwner,float DeltaTime, TArray<FVector>& OutParkourPoints,TArray<UPrimitiveComponent*>& OutWallsToIgnore);
	bool PrepareParkour_Implementation(AActor* StateOwner, float DeltaTime, TArray<FVector>& OutParkourPoints, TArray<UPrimitiveComponent*>& OutWallsToIgnore) { return false; }

	// 실질적인 파쿠르진행 함수
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	void DoParkour(AActor* Owner,const TArray<FVector>& ParkourPoints,const TArray<UPrimitiveComponent*>& WallsToIgnore);
	void DoParkour_Implementation(AActor* Owner, const TArray<FVector>& ParkourPoints, const TArray<UPrimitiveComponent*>& WallsToIgnore) {}

	FGameplayTagContainer& GetLinkedManagedTags() { return ManagedStateTag; };

private:

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayAbility> ParkourGA;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ManagedStateTag;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTParkourComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTParkourComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void Initialize(UObject* Data);

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	void CheckParkourStateTag(const FGameplayTag Tag, int32 Count);

	UFUNCTION(BlueprintCallable)
	void SetParkourProgress(bool On) { bIsParkourInProgress = On; };

	bool IsParkourInProgress() { return bIsParkourInProgress; }

private:

	bool bIsParkourInProgress;

	UPROPERTY()
	TArray<UParkourState*> ParkourStateOnRunning;

	UPROPERTY(Instanced,BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TMap<FGameplayTag,UParkourState*> ManagedParkours;
};
