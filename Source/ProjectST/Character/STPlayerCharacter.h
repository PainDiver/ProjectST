// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "STCharacterBase.h"
#include "NativeGameplayTags.h"
#include "STPlayerInterface.h"
#include "STPlayerCharacter.generated.h"

/**
 * 
 */
class USTDataAsset_Input;

UCLASS()
class PROJECTST_API ASTPlayerCharacter : 
	public ASTCharacterBase,
	public ISTPlayerInterface
{
	GENERATED_BODY()

public:
	ASTPlayerCharacter(const FObjectInitializer& OI);

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void BeginPlay()override;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const;

	// 멀티플레아어 ASC 권장 초기화방식
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// PlayerState OnRep 시 블루프린트 확장용
	UFUNCTION(Client,Reliable)
	void CheckPlayerStateReplication();

	UFUNCTION(Server, Reliable)
	void OnPlayerStateReplicationChecked();


	UFUNCTION()
	void OnSetLockOnTarget(AActor* Actor);
	UFUNCTION()
	void OnRemoveLockOnTarget();
	UFUNCTION(Server,Reliable)
	void SetLockOnTarget_Server(AActor* Actor);
	UFUNCTION(Server, Reliable)
	void RemoveLockOnTarget_Server();


// PlayerInterface
	AActor* GetLockOnTarget_Implementation() const { return LockOnTarget; }

////////////////////
protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	AActor* LockOnTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr < UInputMappingContext> DefaultMappingContext;

	// 인풋액션을 하나로 다 묶어두기 위함, 또한 로직 바인딩을 블루프린트에서 가능하게 하기 위함
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USTDataAsset_Input> InputData;


};
