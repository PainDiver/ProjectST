// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STInteractionSubjectComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScanNewInteractableObject, AActor*, OldScanned, AActor*, NewScanned);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStartInteraction_Subject,USTInteractionObjectComponent*, InteractionObjectComponent,int32, SelectedIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndInteraction_Subject, USTInteractionObjectComponent*, InteractionObjectComponent);


class ASTGimmickActor;
class USTInteractionObjectComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTInteractionSubjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTInteractionSubjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FCanScanDelegate);
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FCanScanDelegate CanScanDelegate;
	UFUNCTION()
	bool CanScan();

	UFUNCTION()
	void ProcessScan();

	UFUNCTION(BlueprintPure)
	AActor* GetCurrentScannedActor() const { return CurrentScannedActor;}

	UFUNCTION()
	void OnScanNewInteractableObject(AActor* New);

	UFUNCTION(BlueprintCallable)
	void RequestProcessInteraction();

	UFUNCTION(Server,Reliable)
	void ProcessInteraction_Server(AActor* ScannedActor,int32 Index);

	UFUNCTION(NetMulticast, Reliable)
	void ProcessInteraction_Multi(AActor* ScannedActor, int32 Index);


	UFUNCTION(BlueprintCallable)
	void RequestEndInteraction(bool bIsSuccess);

	UFUNCTION(Server,Reliable)
	void EndInteraction_Server(bool bIsSuccess);


	UFUNCTION(BlueprintCallable)
	void IncrementSelectedInteraction();

	UFUNCTION(BlueprintCallable)
	void DecrementSelectedInteraction();

	void NotifyOnEndInteraction(USTInteractionObjectComponent* ObjectComp);

	UFUNCTION(BlueprintCallable,Server,Reliable)
	void RequestSetInteractingGimmick_Server(ASTGimmickActor* GimmickActor);

	UFUNCTION(BlueprintCallable)
	void SetInteractingGimmick(ASTGimmickActor* GimmickActor);

	UFUNCTION(BlueprintPure)
	ASTGimmickActor* GetInteractingGimmickActor()const { return InteractingGimmick; }

	UFUNCTION()
	void OnRep_InteractingGimmick(ASTGimmickActor* GimmickActor);

private:

	UPROPERTY(ReplicatedUsing = OnRep_InteractingGimmick)
	ASTGimmickActor* InteractingGimmick;

	UPROPERTY(BlueprintAssignable)
	FOnStartInteraction_Subject OnStartInteractionDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnEndInteraction_Subject OnEndInteractionDelegate;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bDebugScan;

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnScanNewInteractableObject OnScanNewInteractableObjectDelegate;

	FTimerHandle ScanHandle;

	UPROPERTY()
	ACharacter* CharacterOwner;

	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	AActor* CurrentScannedActor;

	UPROPERTY()
	USTInteractionObjectComponent* CurrentInteractionObjectComp;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta = (AllowPrivateAccess = "true"))
	float ScanFrequency = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ScanDistance = 650.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float ScanAngle = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToScan;

	bool bHadScannedActor;
};
