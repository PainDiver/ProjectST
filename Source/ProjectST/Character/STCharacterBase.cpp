// Copyright Epic Games, Inc. All Rights Reserved.

#include "STCharacterBase.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "InputActionValue.h"
#include "GAS/STAbilitySystemComponent.h"
#include "Component/Combo/STComboManagingComponent.h"
#include "GAS/GA/STGameplayAbility.h"
#include "Character/Game/STPlayerState.h"
#include "Character/Component/STInventoryComponent.h"
#include "Component/STCharacterMovementComponent.h"
#include "Character/Component/STMotionWarpingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Game/STNativeGameplayTag.h"
#include "Character/Component/STStateHandlingComponent.h"
#include "GAS/STAttributeSet.h"
#include "Misc/STEventManager.h"
#include "GameplayEffectExtension.h"
#include "Game/STGameStateInterface.h"
#include "GameFramework/GameState.h"
#include "Character/Component/STParkourComponent.h"
#include "Character/Component/STInteractionSubjectComponent.h"
#include "Data/DataTableManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Game/STAssetManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AIController.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ASTCharacterBase

ASTCharacterBase::ASTCharacterBase(const FObjectInitializer& OI)
: Super(OI.SetDefaultSubobjectClass<USTCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	ComboComponent = CreateDefaultSubobject<USTComboManagingComponent>(TEXT("ComboComponent"));
	MotionWarpingComponent = CreateDefaultSubobject<USTMotionWarpingComponent>(TEXT("MotionWarpingComponent"));
	StateHandlingComponent = CreateDefaultSubobject<USTStateHandlingComponent>(TEXT("StateHandlingComponent"));
	ParkourComponent = CreateDefaultSubobject<USTParkourComponent>(TEXT("ParkourComponent"));
	InteractionSubjectComp = CreateDefaultSubobject<USTInteractionSubjectComponent>(TEXT("ScanComponent"));
	InteractionObjectComp = CreateDefaultSubobject<USTInteractionObjectComponent>(TEXT("InteractionObjectComponent"));
}

void ASTCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	SetTeamID(GetTeamID());

	FSTEventDelegate Delegate;
	Delegate.BindDynamic(this, &ThisClass::OnPreparedBothSide);
	USTEventManager::GetEventManager()->RegisterEvent_Subject(this, Event_CharacterPrepared,Delegate ,nullptr,true);
}

void ASTCharacterBase::BeginDestroy()
{
	Super::BeginDestroy();
}

UAbilitySystemComponent* ASTCharacterBase::GetAbilitySystemComponent() const
{
	return ASC_Pointer;
}

void ASTCharacterBase::PointAbilitySystemComponent(USTAbilitySystemComponent* ActualASC)
{
	// 베이스에있는 컴포넌트의 CreateDefaultSubobject를 하위클래스에서 할 시
	// 맵에 배치되었을때 컴포넌트가 누락되는현상 있음
	// 베이스에서는 이걸가리키는 포인터를 들고있으면 캐스팅안해도되는 이점을 누려서 만듬
	ASC_Pointer = ActualASC;
}


void ASTCharacterBase::OnPreparedBothSide(UObject* Data)
{

	if (ASC_Pointer)
	{
		FOnGameplayAttributeValueChange& HealthDelegate = ASC_Pointer->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentHealthAttribute());
		HealthDelegate.AddLambda(
			[this](const FOnAttributeChangeData& Data) {
				OnHealthChanged(Data);
			});
		FOnGameplayAttributeValueChange& StaminaDelegate = ASC_Pointer->GetGameplayAttributeValueChangeDelegate(USTAttributeSet::GetCurrentStaminaAttribute());
		StaminaDelegate.AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnStaminaChanged(Data);
			});

		ASC_Pointer->AbilityCommittedCallbacks.Add(FGenericAbilityDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitted));
		ASC_Pointer->AbilityFailedCallbacks.Add(FAbilityFailedDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilityCommitFailed));
	}

	
	if (HasAuthority())
	{		
		if (USTInventoryComponent* Inventory = ISTCharacterInterface::Execute_GetInventoryComponent(this))
		{
			auto OnAddItemDelegate = FOnAddItem::FDelegate::CreateUObject(this, &ThisClass::OnEquipItem);
			Inventory->GetContainer(EItemContainerType::EQUIPMENT)->GetOnAddItem().Add(OnAddItemDelegate);

			auto OnModifyItemDelegate = FOnAddItem::FDelegate::CreateUObject(this, &ThisClass::OnEquipItem);
			Inventory->GetContainer(EItemContainerType::EQUIPMENT)->GetOnModifyItem().Add(OnModifyItemDelegate);

			auto OnRemoveItemDelegate = FOnAddItem::FDelegate::CreateUObject(this, &ThisClass::OnUnequipItem);
			Inventory->GetContainer(EItemContainerType::EQUIPMENT)->GetOnRemoveItem().Add(OnRemoveItemDelegate);

			Inventory->GetOnUseItemDelegate().AddDynamic(this, &ThisClass::OnUseItem);
		}
	}
}

void ASTCharacterBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{	
	if (Data.OldValue == Data.NewValue)
		return;

	if (Data.NewValue <= 0.f)
	{		
		if (GetWorld())
		{
			AActor* EffectInstigator = Data.GEModData ? Data.GEModData->EffectSpec.GetEffectContext().GetInstigator() : nullptr;
			GetWorld()->GetTimerManager().SetTimerForNextTick(
				[this, EffectInstigator]()
				{
					ISTStateInterface::Execute_OnDead(
						this, 
						EffectInstigator);
					
					ISTGameStateInterface::Execute_OnCharacterDead(
						GetWorld()->GetGameState(),
						this,
						EffectInstigator);
				}
			);
		}
	}
}

void ASTCharacterBase::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
	if (Data.OldValue == Data.NewValue)
		return;

}


//////////////////////////////////////////////////////////////////////////
// Input



void ASTCharacterBase::ProcessMove(const FInputActionInstance& Value)
{
	if (!CanProcessInput(ESTInputType::IT_LOOK, Value))
	{
		return;
	}

	FVector2D MovementVector = Value.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ASTCharacterBase::ProcessLook(const FInputActionInstance& Value)
{
	if (!CanProcessInput(ESTInputType::IT_MOVE,Value))
	{
		return;
	}

	if (HasState_Implementation(CombatState_LockOn))
	{
		return;
	}

	FVector2D LookAxisVector = Value.GetValue().Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

bool ASTCharacterBase::CanJumpInternal_Implementation() const
{	
	if (!CanProcessInput(ESTInputType::IT_JUMP, FInputActionInstance()))
	{
		return false;
	}

	return Super::CanJumpInternal_Implementation() && !IsPlayingRootMotion();
}

void ASTCharacterBase::K2_ProcessInput(ESTInputType InputType, ETriggerEvent TriggerType)
{
	FakeInputInstance.SetTriggerEvent(TriggerType);
	ProcessInput(InputType,FakeInputInstance);
}

void ASTCharacterBase::ProcessInput(ESTInputType InputType, const FInputActionInstance& InputInstance,TFunction<void(bool)>&& CallBack)
{
	if (!CanProcessInput(InputType,InputInstance))
	{
		return;
	}

	ComboComponent->ProcessCombo(InputType, InputInstance, MoveTemp(CallBack));
}

// 세 Skill 시리즈는 콤보컴포넌트를 이용해서 관리할 예정
void ASTCharacterBase::ProcessWeakAttack(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_WEAK_ATTACK, Instance);
}

void ASTCharacterBase::ProcessGuard(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_GUARD, Instance);
}

void ASTCharacterBase::ProcessSway(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SWAY, Instance);
}

void ASTCharacterBase::ProcessSprint(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SPRINT, Instance);
}

void ASTCharacterBase::ProcessSkillQ(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SkillQ, Instance);
}

void ASTCharacterBase::ProcessSkillE(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SkillE, Instance);
}

void ASTCharacterBase::ProcessSkillR(const FInputActionInstance& Instance)
{
	ProcessInput(ESTInputType::IT_SkillR, Instance);
}

void ASTCharacterBase::ProcessLockOn(const FInputActionInstance& Instance)
{
	if (!CanProcessInput(ESTInputType::IT_LOCK_ON, FInputActionInstance()))
	{
		return;
	}

	if (ISTStateInterface::Execute_HasState(this,CombatState_LockOn))
	{
		ISTStateInterface::Execute_RemoveState(this,CombatState_LockOn);
		StateHandlingComponent->RemoveManagedState(CombatState_LockOn);
	}
	else
	{
		ISTStateInterface::Execute_AddState(this,CombatState_LockOn);
		StateHandlingComponent->AddManagedState(CombatState_LockOn);
	}
	//OnProcessLockOn(Instance);
}


void ASTCharacterBase::ProcessInteraction(const FInputActionInstance& Instance)
{
	if (!CanProcessInput(ESTInputType::IT_INTERACTION, Instance))
	{
		return;
	}

	switch (Instance.GetTriggerEvent())
	{
	case ETriggerEvent::Started:
	{
		InteractionSubjectComp->RequestProcessInteraction();
		break;
	}
	case ETriggerEvent::Canceled:
	case ETriggerEvent::Completed:
	{		
		// 클라패킷이 늦게오면 서버가 그 와중에 성공가능성있음.. 
		InteractionSubjectComp->RequestEndInteraction(false);
		break;
	}
	default:
		break;
	}

}

void ASTCharacterBase::ProcessSelectInteraction(const FInputActionInstance& Instance)
{
	if (!CanProcessInput(ESTInputType::IT_SELECTINTERACTION, Instance))
	{
		return;
	}

	if (Instance.GetValue().GetMagnitude() < 0.f)
	{
		InteractionSubjectComp->IncrementSelectedInteraction();
	}
	else
	{
		InteractionSubjectComp->DecrementSelectedInteraction();
	}
}

USTInventoryComponent* ASTCharacterBase::CloneInventoryAndHave(USTInventoryComponent* Inventory, bool bManualAttachment, const FTransform& RelativeTransform)
{
	if (const UWorld* World = GetWorld())
	{
		if (World->bIsTearingDown)
		{
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}

	USTInventoryComponent* NewActorComp = NewObject<USTInventoryComponent>(this, USTInventoryComponent::StaticClass());
	PostCreateBlueprintComponent(NewActorComp);	
	Inventory->CloneAndMove(NewActorComp);	
	FinishAddComponent(NewActorComp, bManualAttachment, RelativeTransform);

	return NewActorComp;
}

void ASTCharacterBase::OnEquipItem(int32 Index, const FReplicatedItemData& Item)
{
	UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
	if (TableManager == nullptr)
		return;

	FItemInfoData ItemInfo;
	if (TableManager->GetTableData<FItemInfoData>(TableManager->ItemInfoDataTable, Item.ItemID, ItemInfo))
	{	
		FItemEquipInfoData EquipInfo;
		if (TableManager->GetTableData<FItemEquipInfoData>(TableManager->EquipItemDataTable, ItemInfo.EquipInfoID, EquipInfo))
		{
			FOnAsyncLoadFinished OnAsyncLoadDelegate;
			OnAsyncLoadDelegate.BindDynamic(this, &ThisClass::OnEquipItemLoaded);
			UObject_OnEquip* EquipData = NewObject<UObject_OnEquip>(this);
			EquipData->ItemData = Item;
			EquipData->EquipInfo = EquipInfo;
			USTAssetManager::LoadAsyncClass(ItemInfo.UseData.UseAbility, OnAsyncLoadDelegate, EquipData);
		}
	}

}

void ASTCharacterBase::OnEquipItemLoaded_Implementation(UObject* LoadedObject, UObject* AdditionalData)
{
	if (UClass* LoadedClass = Cast<UClass>(LoadedObject))
	{
		if (TSubclassOf<UGameplayAbility> GA = LoadedClass)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				ASC->TryActivateAbilityByClass(GA);
				FGameplayEventData Data;
				Data.OptionalObject = AdditionalData;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GA_Action_EquipItem, Data);
			}
		}
	}
}


void ASTCharacterBase::OnUnequipItem(int32 Index, const FReplicatedItemData& Item)
{
	if (USTInventoryComponent* Inventory = ISTCharacterInterface::Execute_GetInventoryComponent(this))
	{
		Inventory->UnregisterEquipmentActor(Item);
	}
	
	FOnAsyncLoadFinished OnAsyncLoadDelegate;
	OnAsyncLoadDelegate.BindDynamic(this, &ThisClass::OnUnequipItemLoaded);
	USTAssetManager::LoadAsyncClass(Item.ItemInfo.UseData.UseEffect, OnAsyncLoadDelegate, nullptr);	
}

void ASTCharacterBase::OnUnequipItemLoaded_Implementation(UObject* LoadedObject, UObject* AddtionalData)
{
	if (UClass* LoadedClass = Cast<UClass>(LoadedObject))
	{
		if (TSubclassOf<UGameplayEffect> GE = LoadedClass)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				ASC->RemoveActiveGameplayEffectBySourceEffect(GE,ASC);
			}
		}
	}

}

void ASTCharacterBase::OnUseItem(USTInventoryComponent* TargetInventory, EItemContainerType TargetInventoryType, const FReplicatedItemData& ItemData)
{
	UDataTableManager* TableManager = UDataTableManager::GetDataTableManager();
	if (TableManager == nullptr)
		return;

	FItemInfoData ItemInfo;
	if (TableManager->GetTableData<FItemInfoData>(TableManager->ItemInfoDataTable, ItemData.ItemID, ItemInfo))
	{
		FOnAsyncLoadFinished OnAsyncLoadDelegate;
		OnAsyncLoadDelegate.BindDynamic(this, &ThisClass::OnUseItemLoaded);
		UObject_OnUseItem* UseData = NewObject<UObject_OnUseItem>(this);
		UseData->ItemData = ItemData;
		UseData->TargetInventoryType = TargetInventoryType;
		UseData->TargetInventory = TargetInventory;
		USTAssetManager::LoadAsyncClass(ItemInfo.UseData.UseAbility, OnAsyncLoadDelegate, UseData);
	}
}

void ASTCharacterBase::OnUseItemLoaded_Implementation(UObject* LoadedObject, UObject* AdditionalData)
{
	if (UClass* LoadedClass = Cast<UClass>(LoadedObject))
	{
		if (TSubclassOf<UGameplayAbility> GA = LoadedClass)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				ASC->TryActivateAbilityByClass(GA);
				FGameplayEventData Data;
				Data.OptionalObject = AdditionalData;
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, GA_Action_UseItem, Data);
			}
		}
	}
}

void ASTCharacterBase::InitializeDefaultSkillSet()
{
	ComboComponent->Initialize(CharacterID);
}

void ASTCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASTCharacterBase, CurrentWeaponType);
}

TMap<ESTInputType, TSubclassOf<UGameplayAbility>> ASTCharacterBase::GetRootComboSet_Implementation(EComboContextState State) const
{
	return ComboComponent->GetRootComboSet(State);
}

void ASTCharacterBase::SetComboContext(const FComboWindowContext& NewWindow)
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->OpenComboWindow(NewWindow);
}

bool ASTCharacterBase::FlushCombo(const FGameplayTagContainer& AllowedTag)
{
	if (ComboComponent == nullptr)
		return false;

	return ComboComponent->FlushCombo(AllowedTag);
}

void ASTCharacterBase::ClearComboContext()
{
	if (ComboComponent == nullptr)
		return;

	ComboComponent->ClearComboWindow();
}


bool ASTCharacterBase::AddState_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer)
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			ASC_Pointer->AddState(Tag);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer)
	{
		ASC_Pointer->RemoveState(Tag);
	}
}

bool ASTCharacterBase::AddState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer && HasAuthority())
	{
		if (StateHandlingComponent->CanAddState(Tag))
		{
			ASC_Pointer->AddState_Replication(Tag);
			return true;
		}
	}
	return false;
}

void ASTCharacterBase::RemoveState_Replication_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer && HasAuthority())
	{
		ASC_Pointer->RemoveState_Replication(Tag);
	}
}

void ASTCharacterBase::OnDead_Implementation(AActor* Killer)
{
}

void ASTCharacterBase::OnKill_Implementation(AActor* Killed)
{
}


FGameplayTagContainer ASTCharacterBase::GetStates_Implementation()
{
	if (ASC_Pointer)
	{
		return ASC_Pointer->GetStates();
	}

	return FGameplayTagContainer::EmptyContainer;
}

bool ASTCharacterBase::IsImmortalState_Implementation()
{
	if (ASC_Pointer->HasState(GE_Buff_Immortal))
	{
		return true;
	}
	
	if (ASC_Pointer->HasState(State_Dead))
	{
		return true;
	}

	if (ASC_Pointer->HasState(CombatState_Evade))
	{
		return true;
	}

	return false;
}

bool ASTCharacterBase::HasState_Implementation(const FGameplayTag& Tag)
{
	if (ASC_Pointer)
	{
		return ASC_Pointer->HasState(Tag);
	}

	return false;
}

void ASTCharacterBase::BindOnStateAddedDelegate(const FGameplayTag& Tag, FOnStateAddedDelgate::FDelegate&& Delegate)
{
	if (USTManagedState* State = StateHandlingComponent->FindState(Tag))
	{
		State->OnStateAddedDelegate.Add(MoveTemp(Delegate));
	}
}

void ASTCharacterBase::BindOnStateTickDelegate(const FGameplayTag& Tag, FOnStateTickDelgate::FDelegate&& Delegate)
{
	if (USTManagedState* State = StateHandlingComponent->FindState(Tag))
	{
		State->OnStateTickDelegate.Add(MoveTemp(Delegate));
	}
}

void ASTCharacterBase::BindOnStateRemoveDelegate(const FGameplayTag& Tag, FOnStateRemovedDelgate::FDelegate&& Delegate)
{
	if (USTManagedState* State = StateHandlingComponent->FindState(Tag))
	{
		State->OnStateRemovedDelegate.Add(MoveTemp(Delegate));
	}
}


USTComboManagingComponent* ASTCharacterBase::GetComboComponent() const
{
	return ComboComponent;
}

USkeletalMeshComponent* ASTCharacterBase::GetMeshComponent_Implementation() const
{
	return GetMesh();
}

AActor* ASTCharacterBase::GetCurrentScannedActor_Implementation() const
{
	return InteractionSubjectComp->GetCurrentScannedActor();
}

USTInventoryComponent* ASTCharacterBase::GetInventoryComponent_Implementation() const
{
	if (ASTPlayerState* PS = Cast<ASTPlayerState>(GetPlayerState()))
	{
		return PS->GetInventoryComponent();
	}

	return GetComponentByClass<USTInventoryComponent>();	
}

ASTPlayerState* ASTCharacterBase::GetSTPlayerState_Implementation() const
{
	return Cast<ASTPlayerState>(GetPlayerState());
}

USTInteractionSubjectComponent* ASTCharacterBase::GetInteractionSubjectComponent_Implementation() const
{
	return InteractionSubjectComp;
}

USTInteractionObjectComponent* ASTCharacterBase::GetInteractionObjectComponent_Implementation() const
{
	return InteractionObjectComp;
}

void ASTCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;	
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		AIController->SetGenericTeamId(NewTeamID);
	}
}

void ASTCharacterBase::SetTeamID(const FGenericTeamId& NewTeamID)
{
	SetGenericTeamId(NewTeamID);
}

