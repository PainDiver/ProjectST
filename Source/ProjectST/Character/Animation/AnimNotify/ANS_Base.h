// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "../STEnum_Anim.h"
#include "ANS_Base.generated.h"

UCLASS(Abstract)
class UANS_ScratchPad : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	UAnimSequenceBase* Animation;

	bool bMarkDead;
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

UCLASS()
class PROJECTST_API UANS_Base : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UANS_Base();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;
	
	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer) { return nullptr; }	
	UShapeComponent* GetCachedShape(UAnimInstance* AnimInstance, ETargetQueryType Type);
	UANS_ScratchPad* GetCachedScratchPad(UAnimInstance* AnimInstance);

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

private:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ENotifyRealm Realm;

	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,meta = (AllowPrivateAccess = "true"))
	int32 ScratchPadKey;	
};
