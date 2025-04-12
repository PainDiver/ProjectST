// Fill out your copyright notice in the Description page of Project Settings.


#include "STCharacterMovementComponent.h"
#include "Character/STStateInterface.h"
#include "Game/STNativeGameplayTag.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


// 부모 매크로,상수 처리
CSV_DEFINE_CATEGORY(STCharacterMovement, true);
DECLARE_CYCLE_STAT(TEXT("STChar ServerMove"), STAT_STCharacterMovementServerMove, STATGROUP_Character);

namespace STCharacterMovementCVars
{
	static int32 NetUseBaseRelativeAcceleration = 1;
	FAutoConsoleVariableRef CVarNetUseBaseRelativeAcceleration(
		TEXT("p.NetUseBaseRelativeAcceleration"),
		NetUseBaseRelativeAcceleration,
		TEXT("If enabled, character acceleration will be treated as relative to dynamic movement bases."));

	static float NetServerMoveTimestampExpiredWarningThreshold = 1.0f;
	FAutoConsoleVariableRef CVarNetServerMoveTimestampExpiredWarningThreshold(
		TEXT("net.NetServerMoveTimestampExpiredWarningThreshold"),
		NetServerMoveTimestampExpiredWarningThreshold,
		TEXT("Tolerance for ServerMove() to warn when client moves are expired more than this time threshold behind the server."),
		ECVF_Default);

	static bool bDeferCharacterMeshMovement = false;
	FAutoConsoleVariableRef CVarDeferCharacterMeshMovement(
		TEXT("p.DeferCharacterMeshMovement"),
		bDeferCharacterMeshMovement,
		TEXT("Optimization - When enabled, defers CharacterMesh move propagation until the end of larger scoped moves. The mesh will still move, but all attached components will wait until all mesh movement is done within the scope."),
		ECVF_Default);
}

struct FScopedMeshMovementUpdate
{
	FScopedMeshMovementUpdate(USkeletalMeshComponent* Mesh, bool bEnabled = true)
		: ScopedMoveUpdate(bEnabled&& STCharacterMovementCVars::bDeferCharacterMeshMovement ? Mesh : nullptr, EScopedUpdate::DeferredUpdates)
	{
	}

private:
	FScopedMovementUpdate ScopedMoveUpdate;
};

struct FScopedCapsuleMovementUpdate : public FScopedMovementUpdate
{
	typedef FScopedMovementUpdate Super;

	FScopedCapsuleMovementUpdate(USceneComponent* UpdatedComponent, bool bEnabled)
		: Super(bEnabled ? UpdatedComponent : nullptr, EScopedUpdate::DeferredUpdates)
	{
	}
};
////////////////////////////////////////////////////////


USTCharacterMovementComponent::USTCharacterMovementComponent()
	:Super()
{
	RotationRate = FRotator(0.f, DefaultStat.RotationRate, 0.f);
	MaxWalkSpeed = DefaultStat.NormalSpeed;
	JumpZVelocity = 380.f;
	AirControl = 0.5f;
	BrakingDecelerationWalking = 3000.f;
	SetNetworkMoveDataContainer(STNetworkMoveDataContainer);
	bCanWalkOffLedges = true;
	SetIsReplicatedByDefault(true);
}

void USTCharacterMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = ELifetimeCondition::COND_SkipOwner;
	DOREPLIFETIME_WITH_PARAMS_FAST(USTCharacterMovementComponent, LastInputVector, Params);
}

void USTCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USTCharacterMovementComponent::ServerMove_PerformMovement(const FCharacterNetworkMoveData& NewData)
{
	SCOPE_CYCLE_COUNTER(STAT_STCharacterMovementServerMove);
	CSV_SCOPED_TIMING_STAT(STCharacterMovement, STCharacterMovementServerMove);

	if (!HasValidData() || !IsActive())
	{
		return;
	}

	const FSTCharacterNetworkMoveData& MoveData = static_cast<const FSTCharacterNetworkMoveData&>(NewData);
	
	const float ClientTimeStamp = MoveData.TimeStamp;

	FVector ClientAccel = MoveData.Acceleration;

	// Convert the move's acceleration to worldspace if necessary
	if (STCharacterMovementCVars::NetUseBaseRelativeAcceleration && MovementBaseUtility::IsDynamicBase(MoveData.MovementBase))
	{
		MovementBaseUtility::TransformDirectionToWorld(MoveData.MovementBase, MoveData.MovementBaseBoneName, MoveData.Acceleration, ClientAccel);
	}

	const uint8 ClientMoveFlags = MoveData.CompressedMoveFlags;
	const FRotator ClientControlRotation = MoveData.ControlRotation;

	FNetworkPredictionData_Server_Character* ServerData = GetPredictionData_Server_Character();
	check(ServerData);

	if (!VerifyClientTimeStamp(ClientTimeStamp, *ServerData))
	{
		const float ServerTimeStamp = ServerData->CurrentClientTimeStamp;
		// This is more severe if the timestamp has a large discrepancy and hasn't been recently reset.
		if (ServerTimeStamp > 1.0f && FMath::Abs(ServerTimeStamp - ClientTimeStamp) > STCharacterMovementCVars::NetServerMoveTimestampExpiredWarningThreshold)
		{
			UE_LOG(LogNetPlayerMovement, Warning, TEXT("ServerMove: TimeStamp expired: %f, CurrentTimeStamp: %f, Character: %s"), ClientTimeStamp, ServerTimeStamp, *GetNameSafe(CharacterOwner));
		}
		else
		{
			UE_LOG(LogNetPlayerMovement, Log, TEXT("ServerMove: TimeStamp expired: %f, CurrentTimeStamp: %f, Character: %s"), ClientTimeStamp, ServerTimeStamp, *GetNameSafe(CharacterOwner));
		}
		return;
	}

	bool bServerReadyForClient = true;
	APlayerController* PC = Cast<APlayerController>(CharacterOwner->GetController());
	if (PC)
	{
		bServerReadyForClient = PC->NotifyServerReceivedClientData(CharacterOwner, ClientTimeStamp);
		if (!bServerReadyForClient)
		{
			ClientAccel = FVector::ZeroVector;
		}
	}

	const UWorld* MyWorld = GetWorld();
	const float DeltaTime = ServerData->GetServerMoveDeltaTime(ClientTimeStamp, CharacterOwner->GetActorTimeDilation(*MyWorld));

	if (!LastInputVector.Equals(MoveData.LastInputVector,0.01f))
	{
		LastInputVector = MoveData.LastInputVector;
		MARK_PROPERTY_DIRTY_FROM_NAME(USTCharacterMovementComponent, LastInputVector, this);
	}

	// Defer all mesh child updates until all movement completes.
	FScopedMeshMovementUpdate ScopedMeshUpdate(CharacterOwner->GetMesh());

	if (DeltaTime > 0.f)
	{
		ServerData->CurrentClientTimeStamp = ClientTimeStamp;
		ServerData->ServerAccumulatedClientTimeStamp += DeltaTime;
		ServerData->ServerTimeStamp = MyWorld->GetTimeSeconds();
		ServerData->ServerTimeStampLastServerMove = ServerData->ServerTimeStamp;

		if (AController* CharacterController = Cast<AController>(CharacterOwner->GetController()))
		{
			CharacterController->SetControlRotation(ClientControlRotation);
		}

		if (!bServerReadyForClient)
		{
			return;
		}

		// Perform actual movement
		if ((MyWorld->GetWorldSettings()->GetPauserPlayerState() == NULL))
		{
			FScopedCapsuleMovementUpdate ScopedMovementUpdate(UpdatedComponent, bEnableScopedMovementUpdates);
			if (PC)
			{
				PC->UpdateRotation(DeltaTime);
			}

			MoveAutonomous(ClientTimeStamp, DeltaTime, ClientMoveFlags, ClientAccel);
		}

		//UE_CLOG(CharacterOwner && UpdatedComponent, LogNetPlayerMovement, VeryVerbose, TEXT("ServerMove Time %f Acceleration %s Velocity %s Position %s Rotation %s GravityDirection %s DeltaTime %f Mode %s MovementBase %s.%s (Dynamic:%d)"),
		//	ClientTimeStamp, *ClientAccel.ToString(), *Velocity.ToString(), *UpdatedComponent->GetComponentLocation().ToString(), *UpdatedComponent->GetComponentRotation().ToCompactString(), *GravityDirection.ToCompactString(), DeltaTime, *GetMovementName(),
		//	*GetNameSafe(GetMovementBase()), *CharacterOwner->GetBasedMovement().BoneName.ToString(), MovementBaseUtility::IsDynamicBase(GetMovementBase()) ? 1 : 0);
	}

	// Validate move only after old and first dual portion, after all moves are completed.
	if (MoveData.NetworkMoveType == FCharacterNetworkMoveData::ENetworkMoveType::NewMove)
	{
		ServerMoveHandleClientError(ClientTimeStamp, DeltaTime, ClientAccel, MoveData.Location, MoveData.MovementBase, MoveData.MovementBaseBoneName, MoveData.MovementMode);
	}
}

FVector USTCharacterMovementComponent::GetLastInputVector_Rep()
{
	if (PawnOwner && PawnOwner->IsLocallyControlled())
	{
		return GetLastInputVector();
	}
	
	return LastInputVector;
}

void USTCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (MovementMode == EMovementMode::MOVE_Falling)
	{
		ISTStateInterface::Execute_AddState(PawnOwner, State_Falling);
	}
	else if (MovementMode == EMovementMode::MOVE_Walking)
	{
		ISTStateInterface::Execute_AddState(PawnOwner, State_Standing);
	}
	else if (MovementMode == EMovementMode::MOVE_Flying)
	{
		ISTStateInterface::Execute_AddState(PawnOwner, State_Flying);
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}
