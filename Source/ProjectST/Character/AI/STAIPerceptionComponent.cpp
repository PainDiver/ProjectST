// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AI/STAIPerceptionComponent.h"
#include "Perception/AISense_Touch.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"


void USTAggroRule::Initialize()
{
	if (DamageAggroSortClass)
	{
		DamageAggroSort = Cast<USTAggroSort>(DamageAggroSortClass->GetDefaultObject());
	}
	if (DamageAggroFilterClass)
	{
		DamageAggroFilter = Cast<USTAggroFilter>(DamageAggroFilterClass->GetDefaultObject());
	}

	if (SightAggroSortClass)
	{
		SightAggroSort = Cast<USTAggroSort>(SightAggroSortClass->GetDefaultObject());
	}
	if (SightAggroFilterClass)
	{
		SightAggroFilter = Cast<USTAggroFilter>(SightAggroFilterClass->GetDefaultObject());
	}

	if (HearingAggroSortClass)
	{
		HearingAggroSort = Cast<USTAggroSort>(HearingAggroSortClass->GetDefaultObject());
	}
	if (HearingAggroFilterClass)
	{
		HearingAggroFilter = Cast<USTAggroFilter>(HearingAggroFilterClass->GetDefaultObject());
	}
}

UWorld* USTAggroSort::GetWorld() const
{
#if WITH_EDITOR
	return GEngine->GetCurrentPlayWorld();
#else
	return Super::GetWorld();
#endif
}

UWorld* USTAggroFilter::GetWorld() const
{
#if WITH_EDITOR
	return GEngine->GetCurrentPlayWorld();
#else
	return Super::GetWorld();
#endif
}


void USTAIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetOwner()->HasAuthority())
	{
		Deactivate();
	}
}

void USTAIPerceptionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USTAIPerceptionComponent::Initialize(TSubclassOf<USTAggroRule> AggroRuleClass, float NewUpdateFrequency)
{
	if (AggroRuleClass)
	{
		AggroRule = AggroRuleClass->GetDefaultObject<USTAggroRule>();
		AggroRule->Initialize();
	}

	UpdateFrequency = NewUpdateFrequency;

	FActorPerceptionUpdatedDelegate::FDelegate UpdateDelegate;
	UpdateDelegate.BindDynamic(this, &ThisClass::OnTargetPerceptionUpdatedEvent);
	OnTargetPerceptionUpdated.Add(UpdateDelegate);

	GetWorld()->GetTimerManager().SetTimer(UpdateTimer,
		FTimerDelegate::CreateUObject(this, &USTAIPerceptionComponent::UpdateAggros),
		UpdateFrequency,
		true);
}

ESenseType USTAIPerceptionComponent::GetStimulusSenseType(const FAIStimulus& Stimulus)
{
	if (Stimulus.Type == UAISense::GetSenseID<UAISense_Sight>())
	{
		return ESenseType::Sight;
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Damage>())
	{
		return ESenseType::Damage;
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Hearing>())
	{
		return ESenseType::Hearing;
	}
	else if (Stimulus.Type == UAISense::GetSenseID<UAISense_Touch>())
	{
		return ESenseType::Touch;
	}

	return ESenseType::None;
}

bool USTAIPerceptionComponent::GetHighestPriorityAggro(FAggroInfo& OutHighestPriorityAggro,ESenseType& OutType)
{
	if (AggroRule == nullptr)
		return false;

	FAggroInfo* HighestPriorityAggro = nullptr;

	FAggroInfo* FirstDamageAggro = nullptr;
	GetHighestPriorityAggroBuffer(AggroRule->DamageAggroFilter,DamageTargets,FirstDamageAggro);

	FAggroInfo* FirstSightAggro = nullptr;
	GetHighestPriorityAggroBuffer(AggroRule->SightAggroFilter, SightTargets, FirstSightAggro);

	FAggroInfo* FirstHearingAggro = nullptr;
	GetHighestPriorityAggroBuffer(AggroRule->HearingAggroFilter, HearingTargets, FirstHearingAggro);

	TArray<TTuple<int32, FAggroInfo*>> PriorityAggros;
	if (FirstDamageAggro)
		PriorityAggros.Add(MakeTuple(AggroRule->DamagePriority, FirstDamageAggro));
	if(FirstSightAggro)
		PriorityAggros.Add(MakeTuple(AggroRule->SightPriority, FirstSightAggro));
	if(FirstHearingAggro)
		PriorityAggros.Add(MakeTuple(AggroRule->HearingPriority, FirstHearingAggro));

	if (PriorityAggros.Num() == 0)
		return false;

	PriorityAggros.Sort([](const auto& A, const auto& B)
		{
			return A.Key > B.Key;
		});

	OutHighestPriorityAggro = *PriorityAggros[0].Value;
	OutType = GetStimulusSenseType(OutHighestPriorityAggro.Stimulus);

	return true;
}

void USTAIPerceptionComponent::GetHighestPriorityAggroBuffer(USTAggroFilter* Filter, TArray<FAggroInfo>& AggroBuffer, FAggroInfo*& OutFirstAggro)
{
	if (Filter)
	{
		for (FAggroInfo& AggroInfo : AggroBuffer)
		{
			if (Filter->OnCheckValidTarget(GetOwner(),AggroInfo))
			{
				OutFirstAggro = &AggroInfo;
				break;
			}
		}
	}
	else if (AggroBuffer.Num() > 0)
	{
		OutFirstAggro = &AggroBuffer[0];
	}
}

void USTAIPerceptionComponent::OnTargetPerceptionUpdatedEvent(AActor* Actor, FAIStimulus Stimulus)
{
	if (AggroRule == nullptr)
		return;


	switch (GetStimulusSenseType(Stimulus))
	{
		case ESenseType::Damage:
		{
			if (AggroRule->DamageAggroSort == nullptr)
				return;
			
			FAggroInfo NewAggro = FAggroInfo{ Actor, MoveTemp(Stimulus), AggroRule->DamageTargetForgetTime,GetWorld()->GetTimeSeconds()};
			int32 Index = DamageTargets.Find(NewAggro);
			if (Index != INDEX_NONE)
			{				
				DamageTargets[Index].Stimulus = MoveTemp(NewAggro.Stimulus);
				DamageTargets[Index].Stimulus.Strength += NewAggro.Stimulus.Strength;
				DamageTargets[Index].RemainingTime = NewAggro.RemainingTime;
				OnTargetAdded.Broadcast(ESenseType::Damage, DamageTargets[Index]);
			}
			else
			{
				DamageTargets.HeapPush(NewAggro, 
					[this](const FAggroInfo& A, const FAggroInfo& B)
					{
						return AggroRule->DamageAggroSort->OnSortTarget(GetOwner(), A, B);
					});
				OnTargetAdded.Broadcast(ESenseType::Damage, NewAggro);
			}
			
			break;
		}
		case ESenseType::Sight:
		{
			if (AggroRule->SightAggroSort == nullptr)
				return;

			FAggroInfo NewAggro = FAggroInfo{ Actor, MoveTemp(Stimulus), -1.f,GetWorld()->GetTimeSeconds() };
			if (Stimulus.WasSuccessfullySensed())
			{				
				int32 Index = SightTargets.Find(NewAggro);
				if (Index != INDEX_NONE)
				{
					SightTargets[Index].Stimulus = NewAggro.Stimulus;
					SightTargets[Index].RemainingTime = -1.f;
					SightTargets[Index].TimeStamp = GetWorld()->GetTimeSeconds();
					OnTargetAdded.Broadcast(ESenseType::Sight, SightTargets[Index]);
				}
				else
				{
					SightTargets.HeapPush(NewAggro,
						[this](const FAggroInfo& A, const FAggroInfo& B)
						{
							return AggroRule->SightAggroSort->OnSortTarget(GetOwner(), A, B);
						});
					OnTargetAdded.Broadcast(ESenseType::Sight, NewAggro);
				}
			}
			else
			{
				int32 Index = SightTargets.Find(NewAggro);
				if (Index != INDEX_NONE)
				{
					SightTargets[Index].RemainingTime = AggroRule->SightTargetForgetTime;
				}
			}
			break;
		}
		case ESenseType::Hearing:
		{
			if (AggroRule->HearingAggroSort == nullptr)
				return;

			FAggroInfo NewAggro = FAggroInfo{Actor, MoveTemp(Stimulus), AggroRule->HearingTargetForgetTime,GetWorld()->GetTimeSeconds() };
			HearingTargets.HeapPush(NewAggro, 
				[this](const FAggroInfo& A, const FAggroInfo& B)
				{
					return AggroRule->HearingAggroSort->OnSortTarget(GetOwner(), A, B);
				});
			OnTargetAdded.Broadcast(ESenseType::Hearing,NewAggro);
			break;
		}
		default:
			break;
	}
}

void USTAIPerceptionComponent::UpdateAggros()
{	
	UpdateAggroBuffer(ESenseType::Damage,DamageTargets);	
	UpdateAggroBuffer(ESenseType::Sight, SightTargets);
	UpdateAggroBuffer(ESenseType::Hearing,HearingTargets);
}

void USTAIPerceptionComponent::UpdateAggroBuffer(ESenseType Type, TArray<FAggroInfo>& Buffer)
{
	if (Buffer.Num() > 0)
	{
		for (FAggroInfo& AggroInfo : Buffer)
		{
			if (AggroInfo.RemainingTime > 0.f)
			{
				AggroInfo.RemainingTime -= UpdateFrequency;
				if (AggroInfo.RemainingTime <= 0.f)
				{
					AggroInfo.bMarkedForDelete = true;
				}
			}
		}

		Buffer.RemoveAll(
			[this, Type](const FAggroInfo& AggroInfo)
			{
				if (AggroInfo.bMarkedForDelete)
				{
					OnTargetRemoved.Broadcast(Type, AggroInfo);
					return true;
				}
				return false;
			});
	}
}
