// Fill out your copyright notice in the Description page of Project Settings.


#include "STNetManager.h"
#include "Networking.h"
#include "Common/TcpSocketBuilder.h"
#include "STNet/Public/STNetSettings.h"


void USTNetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Socket = FTcpSocketBuilder(TEXT("ClientSocket"));
	Socket->SetNonBlocking(true);
	// 랜접속말고 인터넷접속해야 외부인원도 서버로 접속가능함..
	if (const USTNetSettings* NetSetting = GetDefault<USTNetSettings>())
	{
		if (ConnectToServer(NetSetting->ServerIP, NetSetting->ServerPort))
		{
			UE_LOG(LogTemp, Warning, TEXT("STNet Connected"));
			OnConnection();
		}
	}
}

void USTNetManager::Deinitialize()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}
}

TSharedPtr<FBufferArchive> USTNetManager::GeneratePacket(uint32 PaketType,const uint8* Data, uint32 DataSize)
{
	FPacketHeader Header(PaketType, DataSize);
	TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());

	(*Packet) << Header;
	Packet->Append(Data, DataSize);

	return Packet;
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


void USTNetManager::OnConnection()
{
	AsyncTask(ENamedThreads::Type::AnyBackgroundThreadNormalTask,
		[this]()
		{
			while (Socket != nullptr)
			{
				static uint32 OutPendingDataSize = 0;
				if (Socket && Socket->HasPendingData(OutPendingDataSize))
				{
					TSharedPtr<FBufferArchive> Packet = ReceiveData();
					
					// 패킷 여러개 대비			
					FPacketHeader PacketProcessed;
					while (Packet->Tell() < Packet->TotalSize())
					{
						PacketProcessed.ReadPacketHeader(Packet);
						Packet->Seek(sizeof(FPacketHeader));
						// DoJob(PacketProcessed.PacketType,Packet->GetData());
						Packet->Seek(PacketProcessed.PacketSize);
					}
					Packet->Close();
				}
				else
				{
					FPlatformProcess::Sleep(0.1f);
				}
			}
		});
}


const int32 MaxPacketSize = 1024;
TSharedPtr<FBufferArchive> USTNetManager::ReceiveData()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Null Receive Not Fired!"));
		return nullptr;
	}

	uint8 Buffer[MaxPacketSize];
	int32 BytesRead = 0;
	if (Socket->Recv(Buffer, sizeof(Buffer), BytesRead) && BytesRead > 0)
	{
		TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());
		Packet->Append(Buffer,BytesRead);
		return Packet;
	}
	else if (BytesRead == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Connection closed by server"));
		Socket->Close();
	}

	return nullptr;
}

bool USTNetManager::SendData_Internal(TSharedPtr<FBufferArchive> Packet)
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Null SendData Not Fired!"));
		return false;
	}

	//PacketType과 DataSize를 알아내야함
	int32 BytesSent = 0;
	bool bSuccess = Socket->Send(Packet->GetData(), Packet->TotalSize(), BytesSent);
	return bSuccess;
}
