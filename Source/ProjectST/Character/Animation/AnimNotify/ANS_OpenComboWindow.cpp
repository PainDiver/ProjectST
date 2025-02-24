// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_OpenComboWindow.h"



void UANS_OpenComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp == nullptr || MeshComp->GetWorld() == nullptr)
	{
		return;
	}

	
	if (ISTComboEntityInterface* Owner = Cast<ISTComboEntityInterface>(MeshComp->GetOwner()))
	{
		Owner->SetComboContext(ComboWindow);
	}
}

void UANS_OpenComboWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
}

void UANS_OpenComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp == nullptr || MeshComp->GetWorld() == nullptr)
	{
		return;
	}


	if (ISTComboEntityInterface* Owner = Cast<ISTComboEntityInterface>(MeshComp->GetOwner()))
	{
		Owner->ClearComboContext();
	}
}
