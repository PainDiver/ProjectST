// Fill out your copyright notice in the Description page of Project Settings.


#include "STAnimInstance.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"


const uint8 TargetShapeCacheSize = 3;
void USTAnimInstance::NativeBeginPlay()
{
	UAnimInstance::NativeBeginPlay();

	InitializeCachedShapes();
}

void USTAnimInstance::InitializeCachedShapes()
{
	for (int i = 0; i < TargetShapeCacheSize; i++)
	{
		UBoxComponent* Box = NewObject<UBoxComponent>(this);
		USphereComponent* Sphere = NewObject<USphereComponent>(this);
		UCapsuleComponent* Capsule = NewObject<UCapsuleComponent>(this);

		Box->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
		Sphere->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);
		Capsule->SetCollisionObjectType(ECollisionChannel::ECC_Visibility);

		Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Box->RegisterComponent();
		Sphere->RegisterComponent();
		Capsule->RegisterComponent();

		BoxTargetCollision.Add(Box);
		SphereTargetCollision.Add(Sphere);
		CapsuleTargetCollision.Add(Capsule);
	}
}

void USTAnimInstance::CacheScratchPad(const FString& Key, UANS_ScratchPad* ScratchPad)
{
	CachedScratchPad.Add(Key, ScratchPad);
}

void USTAnimInstance::RemoveScratchPad(const FString& Key)
{
	CachedScratchPad.Remove(Key);
}

UANS_ScratchPad* USTAnimInstance::GetCachedScratchPad(const FString& Key)
{
	if (CachedScratchPad.Contains(Key))
	{
		return CachedScratchPad[Key];
	}
	return nullptr;
}

UShapeComponent* USTAnimInstance::GetCachedShape(ETargetQueryType ShapeType)
{
	switch (ShapeType)
	{
		case ETargetQueryType::Box:
		{
			for (UBoxComponent* Box : BoxTargetCollision)
			{
				if (Box->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Box;
				}
			}
			return nullptr;
		}
		case ETargetQueryType::Sphere:
		{
			for (USphereComponent* Sphere : SphereTargetCollision)
			{
				if (Sphere->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Sphere;
				}
			}
			return nullptr;
		}
		case ETargetQueryType::Capsule:
		{
			for (UCapsuleComponent* Capsule : CapsuleTargetCollision)
			{
				if (Capsule->GetCollisionEnabled() == ECollisionEnabled::NoCollision)
				{
					return Capsule;
				}
			}
			return nullptr;
		}
	}

	return nullptr;
}
