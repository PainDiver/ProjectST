// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/STNativeGameplayTag.h"

UE_DEFINE_GAMEPLAY_TAG(State_Standing, "State.Standing");
UE_DEFINE_GAMEPLAY_TAG(State_Falling, "State.Falling");
UE_DEFINE_GAMEPLAY_TAG(State_Lying, "State.Lying");
UE_DEFINE_GAMEPLAY_TAG(State_Dead, "State.Dead");

UE_DEFINE_GAMEPLAY_TAG(CombatState_Attack, "CombatState.Attack");
UE_DEFINE_GAMEPLAY_TAG(CombatState_Evade, "CombatState.Evade");
UE_DEFINE_GAMEPLAY_TAG(CombatState_Guard, "CombatState.Guard");
UE_DEFINE_GAMEPLAY_TAG(CombatState_Sprint, "CombatState.Sprint");
UE_DEFINE_GAMEPLAY_TAG(CombatState_Hit, "CombatState.Hit");
UE_DEFINE_GAMEPLAY_TAG(CombatState_Parry, "CombatState.Parry");
UE_DEFINE_GAMEPLAY_TAG(CombatState_PerfectParry, "CombatState.PerfectParry");


UE_DEFINE_GAMEPLAY_TAG(Input_InputInstance,"Input.InputInstance")


UE_DEFINE_GAMEPLAY_TAG(Hit_Head, "Hit.Head");
UE_DEFINE_GAMEPLAY_TAG(Hit_Right, "Hit.Right");
UE_DEFINE_GAMEPLAY_TAG(Hit_Left, "Hit.Left");
UE_DEFINE_GAMEPLAY_TAG(Hit_Flying, "Hit.Flying");
UE_DEFINE_GAMEPLAY_TAG(Hit_KnockDown, "Hit.KnockDown");
UE_DEFINE_GAMEPLAY_TAG(Hit_Body, "Hit.Body");


UE_DEFINE_GAMEPLAY_TAG(Knockback_Pull, "Knockback.Pull");
UE_DEFINE_GAMEPLAY_TAG(Knockback_PullToHitter, "Knockback.PullToHitter");
UE_DEFINE_GAMEPLAY_TAG(Knockback_Push, "Knockback.Push");

UE_DEFINE_GAMEPLAY_TAG(EC_Damage, "EC.Damage");

UE_DEFINE_GAMEPLAY_TAG(Regen_Stop_Health, "Regen.Stop.Health");
UE_DEFINE_GAMEPLAY_TAG(Regen_Stop_Stamina, "Regen.Stop.Stamina");
