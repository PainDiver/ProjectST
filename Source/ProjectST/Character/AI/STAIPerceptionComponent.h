// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "GameFramework/Actor.h"
#include "STAIPerceptionComponent.generated.h"

UENUM(BlueprintType)
enum class ESenseType : uint8
{
	None,
	Touch,
	Sight,
	Damage,
	Hearing
};

USTRUCT(BlueprintType)
struct FAggroInfo
{
	GENERATED_BODY()

	FAggroInfo() = default;

	FAggroInfo(
		AActor* NewActor,
		FAIStimulus&& NewStimulus,
		float NewRemainingTime,
		double Time)
	{		
		Actor = NewActor;
		Stimulus = MoveTemp(NewStimulus);
		RemainingTime = NewRemainingTime;
		TimeStamp = Time;
	}

	bool operator==(const FAggroInfo& Other)const
	{
		return Actor == Other.Actor;
	}

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere)
	AActor* Actor;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FAIStimulus Stimulus;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float RemainingTime;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	double TimeStamp;

	bool bMarkedForDelete = false;
};


UCLASS(Blueprintable, BlueprintType)
class PROJECTST_API USTAggroSort : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld()const override;


	UFUNCTION(BlueprintNativeEvent)
	bool OnSortTarget(AActor* Querier,const FAggroInfo& A, const FAggroInfo& B);
	bool OnSortTarget_Implementation(AActor* Querier, const FAggroInfo& A, const FAggroInfo& B) { return true; }
};

UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API USTAggroFilter : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld()const override;


	UFUNCTION(BlueprintNativeEvent)
	bool OnCheckValidTarget(AActor* Querier, const FAggroInfo& Aggro);
	bool OnCheckValidTarget_Implementation(AActor* Querier, const FAggroInfo& Aggro) { return true; }
};

UCLASS(Blueprintable,BlueprintType)
class PROJECTST_API USTAggroRule : public UObject
{
	GENERATED_BODY()
public:

	void Initialize();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroSort> DamageAggroSortClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroFilter> DamageAggroFilterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroSort> SightAggroSortClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroFilter> SightAggroFilterClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroSort> HearingAggroSortClass;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<USTAggroFilter> HearingAggroFilterClass;


	UPROPERTY()
	USTAggroSort* DamageAggroSort;
	UPROPERTY()
	USTAggroFilter* DamageAggroFilter;

	UPROPERTY()
	USTAggroSort* SightAggroSort;
	UPROPERTY()
	USTAggroFilter* SightAggroFilter;

	UPROPERTY()
	USTAggroSort* HearingAggroSort;
	UPROPERTY()
	USTAggroFilter* HearingAggroFilter;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageTargetForgetTime = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 DamagePriority = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SightTargetForgetTime = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 SightPriority = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HearingTargetForgetTime = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	uint8 HearingPriority = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetAdded,ESenseType,SenseType ,const FAggroInfo&, AggroInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTargetRemoved, ESenseType, SenseType, const FAggroInfo&, AggroInfo);

UCLASS()
class PROJECTST_API USTAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay()override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;

	void Initialize(TSubclassOf<USTAggroRule> AggroRuleClass, float NewUpdateFrequency);

	UFUNCTION(BlueprintPure)
	ESenseType GetStimulusSenseType(const FAIStimulus& Stimulus);

	UFUNCTION(BlueprintCallable)
	bool GetHighestPriorityAggro( FAggroInfo& OutHighestPriorityAggro, ESenseType& OutType);

	void GetHighestPriorityAggroBuffer(USTAggroFilter* Filter, TArray<FAggroInfo>& AggroBuffer,FAggroInfo*& OutFirstAggro);

	UFUNCTION()
	void OnTargetPerceptionUpdatedEvent(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void UpdateAggros();

	void UpdateAggroBuffer(ESenseType Type,TArray<FAggroInfo>& Buffer);

private:
	UPROPERTY(BlueprintAssignable,meta=(AllowPrivateAccess ="true"))
	FOnTargetAdded OnTargetAdded;

	UPROPERTY(BlueprintAssignable,meta = (AllowPrivateAccess = "true"))
	FOnTargetRemoved OnTargetRemoved;

	UPROPERTY()
	float UpdateFrequency = 0.25f;

	UPROPERTY()
	USTAggroRule* AggroRule;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FAggroInfo> DamageTargets;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FAggroInfo> SightTargets;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FAggroInfo> HearingTargets;

	FTimerHandle UpdateTimer;
};
