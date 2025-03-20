// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Target.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"

void UANS_Target::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UANS_Base::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UShapeComponent* Comp = GetCachedShape(MeshComp->GetAnimInstance(), QueryType))
	{
		Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Comp->SetCollisionResponseToChannels(CollisionType);

		Comp->SetWorldLocation(MeshComp->GetComponentLocation());
		if (AttachingPoint != "")
		{
			FAttachmentTransformRules AttachmentRule{ LocationRule,RotationRule,EAttachmentRule::SnapToTarget,false };
			Comp->AttachToComponent(MeshComp, AttachmentRule, AttachingPoint);
		}

		//FVector LocalOffsetFromMesh =  MeshComp->GetComponentTransform().TransformPosition(Offset);
		//Comp->AddLocalOffset(LocalOffsetFromMesh);
		Comp->AddLocalRotation(OffsetRotation);

		switch (QueryType)
		{
		case ETargetQueryType::Box:
		{
			UBoxComponent* Box = Cast<UBoxComponent>(Comp);
			Box->SetBoxExtent(Extent);
			break;
		}
		case ETargetQueryType::Sphere:
		{
			USphereComponent* Sphere = Cast<USphereComponent>(Comp);
			Sphere->SetSphereRadius(Radius);
			break;
		}
		case ETargetQueryType::Capsule:
		{
			UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Comp);
			Capsule->SetCapsuleRadius(Radius);
			Capsule->SetCapsuleHalfHeight(HalfHeight);
			break;
		}
		}

		FString Key = GetUniqueKey(EventReference.GetNotify());

		if (UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance(), Key)))
		{
			ScratchPad->Collision = Comp;
		}

		Comp->OnComponentBeginOverlap.AddDynamic(this,&UANS_Target::OnOverlap);

	}
}


void UANS_Target::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	UANS_Base::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

#if WITH_EDITORONLY_DATA
	if (bShowShape)
		ShowCollision(MeshComp, EventReference);
#endif
}

void UANS_Target::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance(),GetUniqueKey(EventReference.GetNotify()))))
	{
		if (ScratchPad->Collision)
		{
			ScratchPad->Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ScratchPad->Collision->OnComponentBeginOverlap.Clear();
		}
	}
	UANS_Base::NotifyEnd(MeshComp, Animation, EventReference);
}

FString UANS_Target::GetUniqueKey(const FAnimNotifyEvent* NotifyEvent)
{
	return FString::Printf(TEXT("%s%f%f%d"),
		*NotifyEvent->NotifyName.ToString(),
		NotifyEvent->GetDuration(),
		NotifyEvent->GetTriggerTime(),
		(uint8)QueryType
	);
}

void UANS_Target::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (UTargetBasedAction* Action : ActionAfterTargets)
	{
		if (Action)
		{
			Action->OnTargetFound(OverlappedComponent,OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
		}
	}
}

void UANS_Target::ShowCollision(USkeletalMeshComponent* MeshComp, const FAnimNotifyEventReference& EventReference)
{
	UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance(), GetUniqueKey(EventReference.GetNotify())));
	if (ScratchPad && ScratchPad->Collision)
	{
		FVector ComponentLocation = ScratchPad->Collision->GetComponentLocation();
		FRotator CompoentRotation = ScratchPad->Collision->GetComponentRotation();
		switch (QueryType)
		{
		case ETargetQueryType::Box:
		{
			UBoxComponent* Box = Cast<UBoxComponent>(ScratchPad->Collision);
			FVector BoxExtent = Box->GetScaledBoxExtent(); // 크기 구하기
			DrawDebugBox(
				MeshComp->GetWorld(),                // 월드
				ComponentLocation,         // 위치
				BoxExtent,                 // 크기
				CompoentRotation.Quaternion(),           // 회전 (기본값은 회전 없음)
				FColor::Red,               // 색상
				false,                     // 지속성 (false는 한 프레임만, true는 지속적으로)
				DebugTime,                     // 지속 시간 (-1은 한 프레임 동안만)
				0,                         // 
				1                          // 
			);
			break;
		}
		case ETargetQueryType::Sphere:
		{
			USphereComponent* Sphere = Cast<USphereComponent>(ScratchPad->Collision);
			float SphereRadius = Sphere->GetScaledSphereRadius();
			DrawDebugSphere(
				MeshComp->GetWorld(),                // 월드
				ComponentLocation,         // 위치
				SphereRadius,              // 반지름
				12,                        // 세그먼트 수 (구의 정확도를 결정)
				FColor::Green,             // 색상
				false,                     // 지속성
				DebugTime,                     // 지속 시간
				0,                         // 
				1                         // 
			);
			break;
		}
		case ETargetQueryType::Capsule:
		{
			UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(ScratchPad->Collision);
			float CapsuleRadius = Capsule->GetScaledCapsuleRadius();
			float CapsuleHalfHeight = Capsule->GetScaledCapsuleHalfHeight(); 
			DrawDebugCapsule(
				MeshComp->GetWorld(),                 // 월드
				ComponentLocation,          // 위치
				CapsuleHalfHeight * 2.0f,   // 전체 높이 (반 높이 * 2)
				CapsuleRadius,              // 반지름
				CompoentRotation.Quaternion(),            // 회전
				FColor::Blue,               // 색상
				false,                      // 지속성
				DebugTime,                      // 지속 시간
				0,                          // 두께
				1                           // 깊이
			);
			break;
		}
		}
	}
	else
	{
		FVector ComponentLocation;
		FRotator ComponentRotation = OffsetRotation;
		if (AttachingPoint != "")
		{
			if (LocationRule == EAttachmentRule::SnapToTarget)
			{
				ComponentLocation = MeshComp->GetSocketLocation(AttachingPoint);
			}
			else if (LocationRule == EAttachmentRule::KeepWorld)
			{
				ComponentLocation = MeshComp->GetComponentLocation();
			}

			if (RotationRule == EAttachmentRule::SnapToTarget)
			{
				ComponentRotation = MeshComp->GetSocketRotation(AttachingPoint);
			}
		}

		FVector LocalOffsetFromMesh = MeshComp->GetComponentTransform().TransformPosition(Offset);		
		ComponentLocation = ComponentLocation + LocalOffsetFromMesh;

		switch (QueryType)
		{
		case ETargetQueryType::Box:
		{
			FVector BoxExtent = Extent; // 크기 구하기
			DrawDebugBox(
				MeshComp->GetWorld(),                // 월드
				ComponentLocation,         // 위치
				BoxExtent,                 // 크기
				ComponentRotation.Quaternion(),           // 회전 (기본값은 회전 없음)
				FColor::Red,               // 색상
				false,                     // 지속성 (false는 한 프레임만, true는 지속적으로)
				DebugTime,                     // 지속 시간 (-1은 한 프레임 동안만)
				0,                         // 두께
				1                          // 깊이
			);
			break;
		}
		case ETargetQueryType::Sphere:
		{
			float SphereRadius = Radius;
			DrawDebugSphere(
				MeshComp->GetWorld(),                // 월드
				ComponentLocation,         // 위치
				SphereRadius,              // 반지름
				12,                        // 세그먼트 수 (구의 정확도를 결정)
				FColor::Green,             // 색상
				false,                     // 지속성
				DebugTime,                     // 지속 시간
				0,                         // 두께
				1                          // 깊이
			);
			break;
		}
		case ETargetQueryType::Capsule:
		{
			float CapsuleRadius = Radius;
			float CapsuleHalfHeight = HalfHeight;
			DrawDebugCapsule(
				MeshComp->GetWorld(),                 // 월드
				ComponentLocation,          // 위치
				CapsuleHalfHeight * 2.0f,   // 전체 높이 (반 높이 * 2)
				CapsuleRadius,              // 반지름
				ComponentRotation.Quaternion(),            // 회전
				FColor::Blue,               // 색상
				false,                      // 지속성
				DebugTime,                      // 지속 시간
				0,                          // 두께
				1                           // 깊이
			);
			break;
		}
		}
	
	}

}
