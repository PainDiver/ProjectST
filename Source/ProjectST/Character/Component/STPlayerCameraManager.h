// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "GameplayTagContainer.h"
#include "STPlayerCameraManager.generated.h"

class ASTPlayerCharacter;
class UCameraComponent;
class USpringArmComponent;

UENUM(Blueprintable,BlueprintType)
enum class ECameraProperty : uint8
{
	FOV,
	CameraLag,
	TargetArmLength,
	SocketOffset,
	TargetOffset,
	AspectRatio,
	Max
};


UCLASS(Abstract,Blueprintable,BlueprintType)
class UManagedCameraProperty : public UObject
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld()const override;

	UFUNCTION(BlueprintNativeEvent)
	float UpdateTarget_Float(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm,float DeltaTime);
	float UpdateTarget_Float_Implementation(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime) { return 0.f; }

	UFUNCTION(BlueprintNativeEvent)
	FVector UpdateTarget_Vector(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime);
	FVector UpdateTarget_Vector_Implementation(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, float DeltaTime) { return FVector::ZeroVector; }

	UFUNCTION(BlueprintNativeEvent)
	void PostUpdateTarget_Float(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, float Target, float DeltaTime);
	void PostUpdateTarget_Float_Implementation(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, float Target, float DeltaTime) {};

	UFUNCTION(BlueprintNativeEvent)
	void PostUpdateTarget_Vector(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, const FVector& Target, float DeltaTime);
	void PostUpdateTarget_Vector_Implementation(ASTPlayerCharacter* Character, UCameraComponent* Camera, USpringArmComponent* SpringArm, const FVector& Target, float DeltaTime) {};

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTagContainer States;

	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly)
	ECameraProperty PropertyType;
};

USTRUCT(Blueprintable,BlueprintType)
struct FManualCameraEffect
{
	GENERATED_BODY()

	bool IsValid() { return RemainingTime > 0.f; }

	bool IsFOVValid() { return FOV != -1.f; }
	bool IsCameraLagSpeedValid() { return CameraLagSpeed != -1.f; }
	bool IsTargetArmLengthValid() { return TargetArmLength != -1.f;}
	bool IsSocketOffsetValid() { return SocketOffset != -FVector::OneVector; }
	bool IsTargetOffsetValid() { return TargetOffset != -FVector::OneVector; }
	bool IsAspectRatioValid() { return AspectRatio != -1.f; }

	TSet<ECameraProperty> ControlledProperty;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FOV = -1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CameraLagSpeed = -1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TargetArmLength = -1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SocketOffset = -FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector TargetOffset = -FVector::OneVector;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float AspectRatio = -1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InterpSpeed = 10.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RemainingTime = 0.f;
};

UCLASS()
class PROJECTST_API ASTPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
public:

	ASTPlayerCameraManager();

	virtual void BeginPlay()override;

	UFUNCTION()
	void Initialize(UObject* Data);

	virtual void Tick(float DeltaTime)override;

	void UpdateManualCameraEffect(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetShouldUpdateCameraProperty(bool On) { bShouldUpdateCameraProperty = On; };

	UFUNCTION(BlueprintCallable)
	void SetManualCameraEffect(const FManualCameraEffect& Effect);


	UFUNCTION()
	void OnStateChanged(const FGameplayTag Tag, int32 Count);

private:

	FManualCameraEffect CurrentCameraEffect;

	bool bShouldUpdateCameraProperty = true;

	UPROPERTY()
	TObjectPtr<ASTPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly,meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<UManagedCameraProperty>> ManagedPropertyClass_Blueprint;

	UPROPERTY()
	TArray<UManagedCameraProperty*> ManagedProperties;
};
