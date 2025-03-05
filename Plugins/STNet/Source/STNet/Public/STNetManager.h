// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Serialization/ArrayWriter.h"
#include "STNetPackets.h"
//#include "GeneratedProto.pb.h"
#include "STNetManager.generated.h"


// 이거 되게하느라 개고생했다..

class FSocket;

struct STNET_API FPacketHeader
{
	uint32 PacketType;
	uint32 PacketSize;
	
	FPacketHeader() = default;
	FPacketHeader(uint32 Type,uint32 Size) : PacketType(Type), PacketSize(Size) {}
	
	void ReadPacketHeader(TSharedPtr<FBufferArchive> FilledPacket)
	{
		if (FilledPacket == nullptr)
		{
			return;
		}

		if (uint8* Data = FilledPacket->GetData())
		{
			FMemory::Memcpy(this, Data, sizeof(FPacketHeader));
		}
	}

	friend FArchive& operator<<(FArchive& Ar, FPacketHeader& Header)
	{
		Ar << Header.PacketType;
		Ar << Header.PacketSize;
		return Ar;
	}
};

UCLASS()
class STNET_API USTNetManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:

	virtual void Initialize(FSubsystemCollectionBase& Collection)override;
	virtual void Deinitialize()override;
	
	//패킷 타입,사이즈 공유할 오토젠이 필요할듯
	TSharedPtr<FBufferArchive> GeneratePacket(uint32 PaketType, const uint8* Data, uint32 DataSize);

	bool ConnectToServer(const FString& ServerIP, int32 ServerPort);
	void OnConnection();
	TSharedPtr<FBufferArchive> ReceiveData();

	bool SendData_Internal(TSharedPtr<FBufferArchive> Packet);

private:
	
	FSocket* Socket;
	
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


