// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_FlushComboWindow.h"
#include "Character/Component/Combo/STComboManagingComponent.h"


void UANS_FlushComboWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp == nullptr || MeshComp->GetWorld() == nullptr)
	{
		return;
	}

	if (ISTComboEntityInterface* Owner = Cast<ISTComboEntityInterface>(MeshComp->GetOwner()))
	{
		ISTComboEntityInterface::Execute_FlushCombo(MeshComp->GetOwner());
	}

}
