// Fill out your copyright notice in the Description page of Project Settings.


#include "STNetManager.h"
#include "Networking.h"
#include "Common/TcpSocketBuilder.h"
#include "STNet/Public/STNetSettings.h"
#include "Generated/GeneratedPacketHandler.h"

void USTNetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Socket = FTcpSocketBuilder(TEXT("ClientSocket"));

	PacketHandler = NewObject<USTNetPacketHandler>(this);
	PacketHandler->Initialize();
	// 랜접속말고 인터넷접속해야 외부인원도 서버로 접속가능함..	
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask,
		[this]() 
		{
			if (const USTNetSettings* NetSetting = GetDefault<USTNetSettings>())
			{
				if (ConnectToServer(NetSetting->ServerIP, NetSetting->ServerPort))
				{
					Socket->SetNonBlocking(true);
					UE_LOG(LogTemp, Warning, TEXT("STNet Connected"));
					StartReceiving();
				}
			}
		}
	);
}

void USTNetManager::Deinitialize()
{
	CloseSocket();
}


bool USTNetManager::ConnectToServer(const FString& ServerIP, int32 ServerPort)
{

	FIPv4Address IP;
	FIPv4Address::Parse(ServerIP, IP);

	TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	Addr->SetIp(IP.Value);
	Addr->SetPort(ServerPort);

	return Socket->Connect(*Addr);
}

void USTNetManager::CloseSocket()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}


void USTNetManager::StartReceiving()
{
	while (Socket != nullptr)
	{
		uint32 OutPendingData = 0;
		if (Socket && Socket->HasPendingData(OutPendingData))
		{
			TSharedPtr<FBufferArchive> Packet = ReceiveData();											
			if (Packet == nullptr)
				continue;

			// 패킷 여러개 대비			
			PacketHeader Header;
#if WITH_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("from Server -%d byte"), Packet->TotalSize());
#endif
			uint32 ReadBytes = 0;
			while (Packet->Tell() < Packet->TotalSize())
			{
#if WITH_EDITOR
				UE_LOG(LogTemp, Warning, TEXT("PacketTell -%d byte"), Packet->Tell());
#endif
				Packet->GetData();
				FMemory::Memcpy(&Header, Packet->GetData(), sizeof(PacketHeader));
				Packet->Seek(ReadBytes);
				ReadBytes += sizeof(PacketHeader);
				PacketType Type = static_cast<PacketType>(Header.PacketType);
				if (PacketHandler)
				{
					PacketHandler->DoJob(Type, Packet->GetData(), Type);
				}
				ReadBytes += Header.PacketSize;
				Packet->Seek(ReadBytes);
			}
			Packet->Close();
		}
		else
		{
			FPlatformProcess::Sleep(0.1f);
		}
	}
}


const int32 MaxPacketSize = 1024;
TSharedPtr<FBufferArchive> USTNetManager::ReceiveData()
{
	uint8 Buffer[MaxPacketSize];
	int32 BytesRead = 0;
	bool bReceived = Socket->Recv(Buffer, sizeof(Buffer), BytesRead) && BytesRead > 0;
	if (bReceived)
	{
		if (BytesRead > 0)
		{
			TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());
			Packet->Append(Buffer, BytesRead);
			return Packet;
		}
		else
		{
			return nullptr;
		}
	}
	else if (BytesRead == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection closed by server"));
		CloseSocket();
	}

	return nullptr;
}

bool USTNetManager::SendData_Internal(uint8* Packet,uint32 Size)
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Null SendData Not Fired!"));
		return false;
	}

	//PacketType과 DataSize를 알아내야함
	int32 BytesSent = 0;
	bool bSuccess = Socket->Send(Packet, Size, BytesSent);
	return bSuccess;
}

USTNetPacketHandler* USTNetManager::GetPacketHandler() const
{
	return PacketHandler;
}
