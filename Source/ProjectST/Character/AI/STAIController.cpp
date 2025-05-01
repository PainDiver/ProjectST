// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/STAIController.h"
#include "Character/AI/STAIPerceptionComponent.h"
#include "Data/DataAsset/STDataAsset_AI.h"


ASTAIController::ASTAIController()
	:AAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<USTAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
}

void ASTAIController::BeginPlay()
{
	Super::BeginPlay();

}

void ASTAIController::InitializeAILogic(USTDataAsset_AI* AIData)
{	
	if (AIData == nullptr)
		return;

	UBlackboardComponent* OutBlackboardComp;
	UseBlackboard(AIData->BlackboardData, OutBlackboardComp);
	RunBehaviorTree(AIData->BaseBehaviorTree);
	
	if (UBehaviorTreeComponent* BehaviorTreeComp = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		for (const TPair<FGameplayTag,UBehaviorTree*>& Pair : AIData->DynamicBehaviorTrees)
		{
			BehaviorTreeComp->SetDynamicSubtree(Pair.Key,Pair.Value);
		}
	}

	AIPerceptionComponent->Initialize(AIData->AggroRule,AIData->PerceptionUpdateFrequency);

	AssignedAIData = AIData;
}

bool ASTAIController::HasDynamicBT(const FGameplayTag& Tag)
{
	if (AssignedAIData->DynamicBehaviorTrees.Contains(Tag))
	{
		return true;
	}

	return false;
}

