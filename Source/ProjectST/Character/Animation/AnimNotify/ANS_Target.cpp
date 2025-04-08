// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animation/AnimNotify/ANS_Target.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UWorld* UTargetBasedAction::GetWorld() const
{
#if WITH_EDITOR
	return GEditor ? GEditor->PlayWorld : nullptr;
#else
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
#endif
}


UANS_Target::UANS_Target()
	:UANS_Base()
{
	LimitedCount = -1;
}

void UANS_Target::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	UANS_Base::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	CHECK_ANS_CONDITION_AND_RETURN(MeshComp, Animation, EventReference);


	UShapeComponent* Comp = GetCachedShape(MeshComp->GetAnimInstance(), QueryType);
	if (Comp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Cached Shape Available"));
		return;
	}
	
	Comp->SetCollisionObjectType(CollisionObjectType);
	Comp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Comp->SetCollisionResponseToChannels(CollisionType);

	Comp->SetWorldLocation(MeshComp->GetComponentLocation());
	if (AttachingPoint != "")
	{
		FAttachmentTransformRules AttachmentRule{ LocationRule,RotationRule,EAttachmentRule::SnapToTarget,false };
		Comp->AttachToComponent(MeshComp, AttachmentRule, AttachingPoint);
	}

	FVector LocalOffsetPositionFromMesh =  MeshComp->GetComponentTransform().TransformPosition(Offset);
	Comp->SetWorldLocation(LocalOffsetPositionFromMesh);
	Comp->SetWorldRotation(MeshComp->GetOwner()->GetActorRotation());
	Comp->AddLocalRotation(OffsetRotation);

	TArray<TEnumAsByte<EObjectTypeQuery>> Types;
	GetResponseTypeAsArray(Types);
	TArray<FHitResult> HitResults;
	TArray<AActor*> Ignored{ MeshComp->GetOwner() };

	switch (QueryType)
	{
	case ETargetQueryType::Box:
	{
		UBoxComponent* Box = Cast<UBoxComponent>(Comp);
		Box->SetBoxExtent(Extent);
		UKismetSystemLibrary::BoxTraceMultiForObjects(
			MeshComp->GetWorld(),
			Box->GetComponentLocation(),
			Box->GetComponentLocation(),
			Box->GetScaledBoxExtent(),
			Box->GetComponentRotation(),
			Types,
			false,
			Ignored,
			EDrawDebugTrace::None,
			HitResults,
			true
		);

		break;
	}
	case ETargetQueryType::Sphere:
	{
		USphereComponent* Sphere = Cast<USphereComponent>(Comp);
		Sphere->SetSphereRadius(Radius);
		UKismetSystemLibrary::SphereTraceMultiForObjects(
			MeshComp->GetWorld(),
			Sphere->GetComponentLocation(),
			Sphere->GetComponentLocation(),
			Sphere->GetScaledSphereRadius(),
			Types,
			false,
			Ignored,
			EDrawDebugTrace::None,
			HitResults,
			true
		);

		break;
	}
	case ETargetQueryType::Capsule:
	{
		UCapsuleComponent* Capsule = Cast<UCapsuleComponent>(Comp);
		Capsule->SetCapsuleRadius(Radius);
		Capsule->SetCapsuleHalfHeight(HalfHeight);
		UKismetSystemLibrary::CapsuleTraceMultiForObjects(
			MeshComp->GetWorld(),
			Capsule->GetComponentLocation(),
			Capsule->GetComponentLocation(),
			Capsule->GetScaledCapsuleRadius(),
			Capsule->GetScaledCapsuleHalfHeight(),
			Types,
			false,
			Ignored,
			EDrawDebugTrace::None,
			HitResults,
			true
		);

		break;
	}
	}

	if (UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		ScratchPad->Collision = Comp;

		TArray<FHitResult> ValidHits;
		TSet<AActor*> ValidActors;
		for (const FHitResult& HitResult : HitResults)
		{
			if (ValidActors.Contains(HitResult.GetActor()))
			{
				continue;
			}
			ValidActors.Add(HitResult.GetActor());
			ValidHits.Add(HitResult);
		}
		ScratchPad->AllOverlappedActors = ValidHits;
		for (const FHitResult& HitResult : ScratchPad->AllOverlappedActors)
		{
			OnOverlap(
				Comp,
				HitResult.GetActor(),
				HitResult.GetComponent(),
				HitResult.FaceIndex,
				false,
				HitResult
			);
		}
		if (ScratchPad->AllOverlappedActors.Num() > 0)
			ScratchPad->AllOverlappedActors.Empty();
	}

	Comp->OnComponentBeginOverlap.AddDynamic(this,&UANS_Target::OnOverlap);
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
	UANS_Base::NotifyEnd(MeshComp, Animation, EventReference);

	if (UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance())))
	{
		if (ScratchPad->Collision)
		{
			ScratchPad->Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			ScratchPad->Collision->OnComponentBeginOverlap.Clear();
		}
	}

}

void UANS_Target::GetResponseTypeAsArray(TArray<TEnumAsByte<EObjectTypeQuery>>& OutTypes)
{
	for (int32 i = 0; i < ECollisionChannel::ECC_MAX; i++)
	{
		ECollisionChannel Channel = static_cast<ECollisionChannel>(i);

		if (CollisionType.GetResponse(Channel) >= ECR_Overlap )
		{
			TEnumAsByte<EObjectTypeQuery> ObjectType = UEngineTypes::ConvertToObjectType(Channel);

			// 유효한 ObjectType만 추가
			if (ObjectType != EObjectTypeQuery::ObjectTypeQuery_MAX)
			{
				OutTypes.Add(ObjectType);
			}
		}
	}

}


void UANS_Target::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UAnimInstance* AnimInstance = Cast<UAnimInstance>(OverlappedComponent->GetOuter());
	bool bHitSuccess = false;
	if (OtherActor == AnimInstance->TryGetPawnOwner())
		return;
	
	UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(AnimInstance));
	if (ScratchPad == nullptr)
	{
		return;
	}

	if (ScratchPad->ProcessedActor.Contains(OtherActor))
	{
		return;
	}

	bool bIsActionSucceded = false;
	for (UTargetBasedAction* Action : ActionAfterTargets)
	{			
		if (Action && Action->OnTargetFound(ScratchPad->AllOverlappedActors,GetInterestedScratchPad(AnimInstance), AnimInstance->TryGetPawnOwner(), OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult))
		{	
			bIsActionSucceded = true;
		}				
	}

	if (bIsActionSucceded)
	{
		ScratchPad->ProcessedActor.Add(OtherActor);
		UE_LOG(LogTemp, Warning, TEXT("Something Hit"));
		DecreaseChanceCount(AnimInstance);
	}
}

void UANS_Target::ShowCollision(USkeletalMeshComponent* MeshComp, const FAnimNotifyEventReference& EventReference)
{
	UANS_ScratchPad_Target* ScratchPad = Cast<UANS_ScratchPad_Target>(GetCachedScratchPad(MeshComp->GetAnimInstance()));
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
		FRotator ComponentRotation;
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
		ComponentRotation = ComponentRotation + FRotator(0,90,0) + OffsetRotation;

		if (Offset != FVector::ZeroVector)
		{
			FVector LocalOffsetFromMesh = MeshComp->GetComponentTransform().TransformPosition(Offset);
			ComponentLocation = LocalOffsetFromMesh;
		}

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
