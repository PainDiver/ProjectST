// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../STEnum_Anim.h"
#include "ANS_Base.generated.h"

UCLASS(Abstract,Blueprintable,BlueprintType)
class UANS_ScratchPad : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UAnimSequenceBase* Animation;

	bool bMarkDead;

	int32 RemainingCount;
};

UENUM()
enum class ENotifyRealm
{
	All,
	AuthorityOnly,
	LocalOwnerOnly,
	ExceptDedicateServer,	
	ExceptRemote
};

UCLASS(Blueprintable,BlueprintType,EditInlineNew)
class UANS_Condition : public UObject
{
	GENERATED_BODY()

public:

	virtual UWorld* GetWorld()const override;

	UFUNCTION(BlueprintImplementableEvent)
	bool CanProcess(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

};


#define CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference)\
if (!CheckCondition(MeshComp, Animation, EventReference))\
{\
	return;\
}\

#define CHECK_ANS_REALM_CONDITION_AND_RETURN(MeshComp, EventReference)\
if (!CheckRealmCondition(MeshComp, EventReference))\
{\
return; \
}\


UCLASS()
class PROJECTST_API UANS_Base : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UANS_Base();

	UFUNCTION(BlueprintCallable)
	bool CheckCondition(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);

	UFUNCTION(BlueprintCallable)
	bool CheckRealmCondition(USkeletalMeshComponent* MeshComp, const FAnimNotifyEventReference& EventReference);


	UFUNCTION(BlueprintCallable)
	void DecreaseChanceCount(UAnimInstance* AnimInstance);

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;
	
	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer) { return nullptr; }	
	UShapeComponent* GetCachedShape(UAnimInstance* AnimInstance, ETargetQueryType Type);
	
	UFUNCTION(BlueprintCallable)
	UANS_ScratchPad* GetCachedScratchPad(UAnimInstance* AnimInstance);

	UFUNCTION(BlueprintCallable)
	UANS_ScratchPad* GetInterestedScratchPad(UAnimInstance* AnimInstance);

	template<typename T>
	T* GetOwningCharacter(USkeletalMeshComponent* MeshComp)
	{
		if (MeshComp)
		{
			return Cast<T>(MeshComp->GetOwner());
		}
		return nullptr;
	}

	virtual ENotifyRealm GetNotifyRealm()const { return Realm; };

	virtual void PostLoad();

	virtual void PostDuplicate(bool bDuplicateForPIE);

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Condition,meta = (AllowPrivateAccess = "true"))
	bool bShouldMeetAllCondition = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere,Instanced, Category = Condition, meta = (AllowPrivateAccess = "true"))
	TArray<UANS_Condition*> Conditions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ENotifyRealm Realm;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Condition, meta = (AllowPrivateAccess = "true"))
	int32 LimitedCount = -1;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,meta = (AllowPrivateAccess = "true"))
	int32 ScratchPadKey;	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 ANSUniqueKeyForScratchPad;

};
