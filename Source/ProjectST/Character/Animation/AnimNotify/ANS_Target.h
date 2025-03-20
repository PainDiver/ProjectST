// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Animation/AnimNotify/ANS_Base.h"
#include "ANS_Target.generated.h"


UCLASS(Abstract,Blueprintable,BlueprintType,EditInlineNew)
class UTargetBasedAction : public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetFound(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

UCLASS()
class UANS_ScratchPad_Target : public UANS_ScratchPad
{
	GENERATED_BODY()
public:
	UPROPERTY()
	UShapeComponent* Collision;
};

UCLASS()
class PROJECTST_API UANS_Target : public UANS_Base
{
	GENERATED_BODY()
public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)override;	
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

	virtual FString GetUniqueKey(const FAnimNotifyEvent* NotifyEvent);

	virtual UANS_ScratchPad* CreateScratchPad(UObject* Outer) 
	{
		return NewObject<UANS_ScratchPad_Target>(Outer);
	}

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Shape)
	ETargetQueryType QueryType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shape, meta = (EditCondition = "QueryType == ETargetQueryType::Box"))
	FVector Extent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shape, meta = (EditCondition = "QueryType == ETargetQueryType::Sphere || QueryType == ETargetQueryType::Capsule"))
	float Radius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Shape, meta = (EditCondition = "QueryType == ETargetQueryType::Capsule"))
	float HalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Offset)
	FVector Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Offset)
	FRotator OffsetRotation;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment)
	FName AttachingPoint = NAME_None;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = Attachment,meta = (EditCondition = "AttachingPoint != NAME_None "))
	EAttachmentRule LocationRule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attachment, meta = (EditCondition = "AttachingPoint != NAME_None "))
	EAttachmentRule RotationRule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Collision)
	FCollisionResponseContainer CollisionType;


	UPROPERTY(EditAnywhere,BlueprintReadWrite, Instanced)
	TArray<UTargetBasedAction*> ActionAfterTargets;

#if	WITH_EDITORONLY_DATA

	void ShowCollision(USkeletalMeshComponent* MeshComp, const FAnimNotifyEventReference& EventReference);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bShowShape = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DebugTime = 0.f;

#endif
};
