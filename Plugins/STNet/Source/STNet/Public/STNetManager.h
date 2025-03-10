// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/ArrayWriter.h"
#include "STNetManager.generated.h"


// 이거 되게하느라 개고생했다..

class FSocket;
class USTNetPacketHandler;
class FNetThread;

UCLASS()
class STNET_API USTNetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	USTNetManager();

	~USTNetManager();

	static USTNetManager* Get()
	{
		if (GEngine == nullptr)
			return nullptr;

		UGameInstance* GameInstance = GEngine->GetCurrentPlayWorld()->GetGameInstance();
		if (GameInstance == nullptr)
			return nullptr;

		return GameInstance->GetSubsystem<USTNetManager>();
	}

	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;

	
	bool ConnectToServer(const FString& ServerIP, int32 ServerPort);
	
	void CloseSocket();

	void StartReceiving();
	TSharedPtr<FBufferArchive> ReceiveData();

	bool SendData_Internal(uint8* Packet,uint32 Size);


	UFUNCTION(BlueprintCallable)
	USTNetPacketHandler* GetPacketHandler()const;

private:
	bool bShouldStop;
	
	FSocket* Socket;
	
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,meta = (AllowPrivateAccess = "true"))
	USTNetPacketHandler* PacketHandler;

	FNetThread* Connector;
	FNetThread* Listener;
};


class FNetThread : public FRunnable
{
public:

	FNetThread() = default;

	FNetThread(TFunction<void()>&& WorkFunc)
	{
		Callback = MoveTemp(WorkFunc);
	}

	~FNetThread()
	{
		if (Thread)
		{
			delete Thread;
			Thread = nullptr;
		}
	}

	virtual uint32 Run() override
	{
		bRunThread = true;
		Callback();
		bRunThread = false;
		return 0;
	}

	bool IsRunningThread()const { return bRunThread; }

	//FRunnableThread* 의 경우 내부에서 관리되기때문에 수동으로 제거 x
	// 킬 호출 시,
	virtual void Stop() override
	{
	}

	// Run에서 탈출 시
	virtual void Exit() override
	{
	}

	void StartThread(const TCHAR* ThreadName)
	{
		Thread = FRunnableThread::Create(this, ThreadName, 0, TPri_Normal);
	}

private:

	FRunnableThread* Thread;
	TFunction<void()> Callback;
	bool bRunThread;
};