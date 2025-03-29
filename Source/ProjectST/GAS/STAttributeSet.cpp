#include "STAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "Data/DataTableManager.h"
#include "Character/STStateInterface.h"
#include "GameplayEffectExtension.h"


USTAttributeSet::USTAttributeSet()
{
	//InitMaxHealth();
	//InitCurrentHealth();
}

void USTAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, MaxHealth, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, CurrentHealth, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, MaxStamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, CurrentStamina, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, CriticalChance, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, CriticalDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, HealthRegen, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, StaminaRegen, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(USTAttributeSet, Defense, COND_OwnerOnly, REPNOTIFY_Always);


}

void USTAttributeSet::OnInitializeAttributes(uint32 CharacterID)
{
	if (UDataTableManager* DataManager = UDataTableManager::GetDataTableManager())
	{
		FCharacterBaseStat CharacterBaseStat;
		DataManager->GetCharacterStat(CharacterID,CharacterBaseStat);
		InitMaxHealth(CharacterBaseStat.MaxHealth);
		InitCurrentHealth(CharacterBaseStat.MaxHealth);
		InitMaxStamina(CharacterBaseStat.MaxStamina);
		InitCurrentStamina(CharacterBaseStat.MaxStamina);
		InitCriticalChance(CharacterBaseStat.CriticalChance);
		InitCriticalDamage(CharacterBaseStat.CriticalDamage);
		InitHealthRegen(CharacterBaseStat.HealthRegen);
		InitStaminaRegen(CharacterBaseStat.StaminaRegen);
		InitDefense(CharacterBaseStat.Defense);
	}
}

void USTAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	const FGameplayAttributeData* DataChanged = Attribute.GetGameplayAttributeData(this);
	if (DataChanged == &CurrentHealth)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxHealth.GetCurrentValue());
	}
	else if (DataChanged == &CurrentStamina)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
}

void USTAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	
	FGameplayAttributeData* DataChanged = Attribute.GetGameplayAttributeData(this);
	if (DataChanged == &CurrentHealth)
	{
		NewValue = FMath::Clamp(NewValue, 0.f,MaxHealth.GetCurrentValue());
	}
	else if (DataChanged == &CurrentStamina)
	{
		NewValue = FMath::Clamp(NewValue, 0.f, MaxStamina.GetCurrentValue());
	}
}

void USTAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void USTAttributeSet::PostAttributeBaseChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (AActor* Avatar = GetOwningAbilitySystemComponent()->GetAvatarActor())
	{
		ISTStateInterface::Execute_OnAttributeChanged(Avatar,Attribute, OldValue, NewValue);
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

void USTAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, MaxStamina, OldMaxStamina);
}

void USTAttributeSet::OnRep_CurrentStamina(const FGameplayAttributeData& OldCurrentStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, CurrentStamina, OldCurrentStamina);
}

void USTAttributeSet::OnRep_HealthRegen(const FGameplayAttributeData& OldHealthRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, HealthRegen, OldHealthRegen);
}

void USTAttributeSet::OnRep_StaminaRegen(const FGameplayAttributeData& OldStaminaRegen)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, StaminaRegen, OldStaminaRegen);
}

void USTAttributeSet::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, CriticalChance, OldCriticalChance);
}

void USTAttributeSet::OnRep_CriticalDamage(const FGameplayAttributeData& OldCriticalDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, CriticalDamage, OldCriticalDamage);
}

void USTAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(USTAttributeSet, Defense, OldDefense);
}
