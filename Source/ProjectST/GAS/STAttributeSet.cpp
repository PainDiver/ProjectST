#include "STAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Game/DataTableManager.h"

USTAttributeSet::USTAttributeSet()
{
	//InitMaxHealth();
	//InitCurrentHealth();
}

void USTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, CurrentHealth, COND_None, REPNOTIFY_Always);
}

void USTAttributeSet::OnInitializeAttributes(uint32 CharacterID)
{
	if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager(this))
	{
		FCharacterBaseStat CharacterBaseStat;
		DataManager->GetCharacterStat(CharacterID,CharacterBaseStat);
		InitMaxHealth(CharacterBaseStat.MaxHealth);
		InitCurrentHealth(CharacterBaseStat.MaxHealth);
	}
}

void USTAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, MaxHealth, OldMaxHealth);
}

void USTAttributeSet::OnRep_CurrentHealth(const FGameplayAttributeData& OldCurrentHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet,CurrentHealth, OldCurrentHealth);
}
