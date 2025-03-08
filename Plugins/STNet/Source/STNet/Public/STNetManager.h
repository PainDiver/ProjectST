// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/ArrayWriter.h"
#include "STNetManager.generated.h"


// 이거 되게하느라 개고생했다..

class FSocket;
class USTNetPacketHandler;



UCLASS()
class STNET_API USTNetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

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
	
	FSocket* Socket;
	
	UPROPERTY(BlueprintReadOnly,VisibleAnywhere,meta = (AllowPrivateAccess = "true"))
	USTNetPacketHandler* PacketHandler;

public:

	// 패킷 자동화
	// Keyword <SendStruct, ResponseStruct>
	//UFUNCTION(BlueprintCallable)
	//FORCEINLINE void SendData_SendStruct(const FSendStruct& Data)
	//{
	//	TSharedPtr<FBufferArchive> Packet = GeneratePacket(&Data, sizeof(Data));
	//	SendData_Internal(Packet);
	//}
	//UPROPERTY(BlueprintAssignable)
	//FResponseDelegate_ResponseStruct OnResponse_ResponseStruct;
};


