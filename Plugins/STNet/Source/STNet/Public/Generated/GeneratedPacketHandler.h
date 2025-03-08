
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
	TMap<PacketType, TFunction<void(uint8*,uint32)>> Delegates;
	void Initialize()
	{
		
		Delegates.Add(PacketType::PT_SC_LOGIN, 
		[this](uint8 * Data, uint16 PacketSize)
		{
			FSC_Packet_Login Message;
			FMemory::Memmove(&Message, Data, PacketSize);
			Dele_SC_Packet_Login.Broadcast(Message); 
		});
		Delegates.Add(PacketType::PT_SC_MATCH, 
		[this](uint8 * Data, uint16 PacketSize)
		{
			FSC_Packet_Match Message;
			FMemory::Memmove(&Message, Data, PacketSize);
			Dele_SC_Packet_Match.Broadcast(Message); 
		});
	}
	void DoJob(PacketType PacketType,uint8* Data,uint32 PacketSize)
	{
		if (Delegates.Contains(PacketType))
		{
			AsyncTask(ENamedThreads::GameThread, [this, PacketType, Data, PacketSize]()
			{
				Delegates[PacketType](Data, PacketSize);
			});
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
		return USTNetManager::Get()->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
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
		return USTNetManager::Get()->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
	};


UPROPERTY(BlueprintAssignable)
FDele_SC_Packet_Login Dele_SC_Packet_Login;

UPROPERTY(BlueprintAssignable)
FDele_SC_Packet_Match Dele_SC_Packet_Match;
;
};

