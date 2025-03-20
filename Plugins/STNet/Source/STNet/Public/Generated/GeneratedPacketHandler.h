
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GeneratedStructs.h"
#include "STNetManager.h"
#include "GeneratedPacketConverter.h"
#include "GeneratedPacketHandler.generated.h"

struct PacketHeader
{
	PacketType PacketType;
	uint32 PacketSize; 
};

UCLASS(Blueprintable,BlueprintType)
class STNET_API USTNetPacketHandler : public UObject
{
	GENERATED_BODY()
public:	
	TMap<PacketType, TFunction<void(const uint8*,uint32)>> Delegates;
	void Initialize()
	{
		
		Delegates.Add(PacketType::PT_SC_LOGIN, 
		[this](const uint8 * Data, uint16 PacketSize)
		{
			FSC_Packet_Login Message;
			google::protobuf::Arena arena;
			SC_Packet_Login* ProtobufPacket = google::protobuf::Arena::Create<SC_Packet_Login>(&arena);
			ProtobufPacket->ParseFromArray(Data, PacketSize);
			ToUnrealStruct(&Message,*ProtobufPacket);			
			Dele_SC_Packet_Login.Broadcast(Message); 
		});
		Delegates.Add(PacketType::PT_SC_MATCH, 
		[this](const uint8 * Data, uint16 PacketSize)
		{
			FSC_Packet_Match Message;
			google::protobuf::Arena arena;
			SC_Packet_Match* ProtobufPacket = google::protobuf::Arena::Create<SC_Packet_Match>(&arena);
			ProtobufPacket->ParseFromArray(Data, PacketSize);
			ToUnrealStruct(&Message,*ProtobufPacket);			
			Dele_SC_Packet_Match.Broadcast(Message); 
		});
		Delegates.Add(PacketType::PT_SD_DEDIExit, 
		[this](const uint8 * Data, uint16 PacketSize)
		{
			FSD_Packet_DediExit Message;
			google::protobuf::Arena arena;
			SD_Packet_DediExit* ProtobufPacket = google::protobuf::Arena::Create<SD_Packet_DediExit>(&arena);
			ProtobufPacket->ParseFromArray(Data, PacketSize);
			ToUnrealStruct(&Message,*ProtobufPacket);			
			Dele_SD_Packet_DediExit.Broadcast(Message); 
		});
	}
	void DoJob(PacketType PacketType,const TArray<uint8>& Data,uint32 PacketSize)
	{
		if (Delegates.Contains(PacketType))
		{
			Delegates[PacketType](Data.GetData(), PacketSize);
		}
	}


	UFUNCTION(BlueprintCallable)
	bool SendData_FCS_Packet_Login(const FCS_Packet_Login& Data)
	{		
		CS_Packet_Login Packet;
		ToProtobuf(&Packet, Data);
		PacketHeader Header;
		Header.PacketType = PT_CS_LOGIN;
		Header.PacketSize = Packet.ByteSizeLong();
		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(sizeof(PacketHeader) + Packet.ByteSizeLong());
		FMemory::Memcpy(Buffer.GetData(),&Header,sizeof(PacketHeader));		
		Packet.SerializeToArray(Buffer.GetData()+sizeof(PacketHeader), Packet.ByteSizeLong());
		return USTNetManager::Get(this)->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
	};
	UFUNCTION(BlueprintCallable)
	bool SendData_FCS_Packet_Match(const FCS_Packet_Match& Data)
	{		
		CS_Packet_Match Packet;
		ToProtobuf(&Packet, Data);
		PacketHeader Header;
		Header.PacketType = PT_CS_MATCH;
		Header.PacketSize = Packet.ByteSizeLong();
		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(sizeof(PacketHeader) + Packet.ByteSizeLong());
		FMemory::Memcpy(Buffer.GetData(),&Header,sizeof(PacketHeader));		
		Packet.SerializeToArray(Buffer.GetData()+sizeof(PacketHeader), Packet.ByteSizeLong());
		return USTNetManager::Get(this)->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
	};
	UFUNCTION(BlueprintCallable)
	bool SendData_FDS_Packet_Dedi(const FDS_Packet_Dedi& Data)
	{		
		DS_Packet_Dedi Packet;
		ToProtobuf(&Packet, Data);
		PacketHeader Header;
		Header.PacketType = PT_DS_DEDI;
		Header.PacketSize = Packet.ByteSizeLong();
		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(sizeof(PacketHeader) + Packet.ByteSizeLong());
		FMemory::Memcpy(Buffer.GetData(),&Header,sizeof(PacketHeader));		
		Packet.SerializeToArray(Buffer.GetData()+sizeof(PacketHeader), Packet.ByteSizeLong());
		return USTNetManager::Get(this)->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
	};


UPROPERTY(BlueprintAssignable)
FDele_SC_Packet_Login Dele_SC_Packet_Login;

UPROPERTY(BlueprintAssignable)
FDele_SC_Packet_Match Dele_SC_Packet_Match;

UPROPERTY(BlueprintAssignable)
FDele_SD_Packet_DediExit Dele_SD_Packet_DediExit;
;
};

