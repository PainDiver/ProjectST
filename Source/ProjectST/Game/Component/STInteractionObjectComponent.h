// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "STInteractionObjectComponent.generated.h"

class USphereComponent;
class UCapsuleComponent;
class UBoxComponent;
class USTInteractionSubjectComponent;

USTRUCT(BlueprintType)
struct PROJECTST_API FInteractionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Desc;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float InteractionTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 interactionMaxCount = MAX_int32;
};


USTRUCT(BlueprintType)
struct PROJECTST_API FInteractionContext
{
	GENERATED_BODY()
	FInteractionContext() = default;

	FInteractionContext(int32 Count)
	{
		RemainingCount = Count;
	}

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	int32 RemainingCount;
};

USTRUCT(BlueprintType)
struct PROJECTST_API FInteractionProgress
{
	GENERATED_BODY()

	void Set(ACharacter* NewInteractor,
		USTInteractionSubjectComponent* SubjectComponent,
		uint32 NewInteractionIndex,
		FInteractionInfo* NewInteractionInfo,
		FInteractionDelegateSets* NewInteractionDelegates)
	{
		Interactor = NewInteractor;
		InterectionSubjectComponent = SubjectComponent;
		InteractionIndex = NewInteractionIndex;
		CurrentInteractionInfo = NewInteractionInfo;
		CurrentInteractionDelegates = NewInteractionDelegates;
	}

	void Reset()
	{
		Interactor = nullptr;
		InterectionSubjectComponent = nullptr;
		InteractionIndex = -1;
		CurrentInteractionInfo = nullptr;
		CurrentInteractionDelegates = nullptr;
		ElapsedInteractionTime = 0.f;
	}

	FORCEINLINE bool IsOnInteraction()const {return InteractionIndex != INDEX_NONE; }
	FORCEINLINE void OnInteractionDone() { InteractionIndex = INDEX_NONE; }

	UPROPERTY()
	ACharacter* Interactor;

	UPROPERTY()
	USTInteractionSubjectComponent* InterectionSubjectComponent;

	UPROPERTY()
	uint32 InteractionIndex = INDEX_NONE;

	FInteractionInfo* CurrentInteractionInfo;

	FInteractionDelegateSets* CurrentInteractionDelegates;

	UPROPERTY()
	float ElapsedInteractionTime;
};

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FCanInteractDelegate, AActor*, Interactor, const FInteractionInfo&, InteractionInfo);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnStartInteraction, AActor*, Interactor, const FInteractionInfo&, InteractionInfo);
DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(bool, FCanInteractOnTickDelegate, AActor*, Interactor, const FInteractionInfo&, InteractionInfo);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnTickInteractionDelegate, AActor*, Interactor, float, DeltaTime, const FInteractionInfo&, InteractionInfo);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnEndInteractionDelegate, AActor*, Interactor, bool, bIsSuccess, const FInteractionInfo&, InteractionInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFailInteractionDelegate, AActor*, Interactor, const FInteractionInfo&, InteractionInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInteractionSelected, int32,OldIndex,int32, NewIndex);


USTRUCT(BlueprintType)
struct FInteractionDelegateSets
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = InteractionDelegates)
	FCanInteractDelegate CanInteractDelegate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = InteractionDelegates)
	FOnStartInteraction OnStartInteractionDelegate;

	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, category = InteractionDelegates)
	FCanInteractOnTickDelegate CanInteractOnTickDelegate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = InteractionDelegates)
	FOnTickInteractionDelegate OnTickDelegate;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, category = InteractionDelegates)
	FOnEndInteractionDelegate OnEndInteractionDelegate;

};

UCLASS( Blueprintable,ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTST_API USTInteractionObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USTInteractionObjectComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintPure)
	bool CanInteract(AActor* Interactor,int32 Index);

	UFUNCTION(BlueprintCallable)
	void OnInteractionFail(AActor* Interactor, int32 Index);

	UFUNCTION(BlueprintCallable,BlueprintAuthorityOnly)
	void StartInteraction(AActor* Interactor, int32 Index);
	
	UFUNCTION(BlueprintPure)
	bool CanInteractOnTick(AActor* Interactor);
	
	void OnTickInteraction(AActor* Interactor,float DeltaTime);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void EndInteraction(AActor* Interactor, bool bIsSuccess);

	UFUNCTION(Server, Reliable)
	void EndInteraction_Server(AActor* Interactor, bool bIsSuccess);


	UFUNCTION(NetMulticast,Reliable)
	void EndInteraction_Multi(AActor* Interactor, bool bIsSuccess);


	UFUNCTION(BlueprintPure)
	FORCEINLINE ACharacter* GetInteractor() const { return InteractionProgress.Interactor; }

	UFUNCTION(BlueprintPure)
	bool IsInteracting()const { return InteractionProgress.IsOnInteraction(); };

	UFUNCTION(BlueprintPure)
	FORCEINLINE FInteractionProgress& GetInteractionProgress() { return InteractionProgress; }

	UFUNCTION(BlueprintPure)
	int32 GetCurrentSelectedIndex()const { return CurrentSelectedIndex; }

	UFUNCTION(BlueprintCallable)
	void SetCurrentSelectedIndex(int32 Index);

	UFUNCTION(BlueprintPure)
	FORCEINLINE int32 GetInteractionLastIndex()const{ return InteractionInfos.Num()-1; }

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, BlueprintAssignable,meta = (AllowPrivateAccess = "true"))
	FOnInteractionSelected OnInteractionIndexSelectedDelegate;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnFailInteractionDelegate OnFailInteractionDelegate;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	int32 CurrentSelectedIndex = 0;

	UPROPERTY(BlueprintReadWrite,EditAnywhere,meta = (AllowPrivateAccess = "true"))
	TArray<FInteractionInfo> InteractionInfos;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FInteractionDelegateSets> InteractionDelegates;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<FInteractionContext> InteractionContexts;

	UPROPERTY()
	FInteractionProgress InteractionProgress;

};
