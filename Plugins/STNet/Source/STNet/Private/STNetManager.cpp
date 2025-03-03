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
	// 근데 여기에 공인아이피 써놓으면 깃허브에 못올리는데~~
	PacketSequence = 0;
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

TSharedPtr<FBufferArchive> USTNetManager::GeneratePacket(const uint8* Data, uint32 DataSize)
{
	FPacketHeader Header(sizeof(FPacketHeader) + DataSize,PacketSequence++);
	TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());

	// 패킷사이즈 =  8바이트 + 데이터사이즈
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
			while (true)
			{
				static uint32 OutPendingDataSize = 0;
				if (Socket && Socket->HasPendingData(OutPendingDataSize))
				{
					FString PacketReceived = ReceiveData();
					TSharedPtr<FBufferArchive> Packet = MakeShareable(new FBufferArchive());
					(*Packet) << PacketReceived;

					// 패킷 여러개 대비			
					FPacketHeader PacketProcessed;
					while (Packet->Tell() < Packet->TotalSize())
					{
						PacketProcessed.ReadPacketHeader(Packet);
						Packet->Seek(sizeof(FPacketHeader));
						// DoJob(PacketSequence,Packet->GetData());
						Packet->Seek(PacketProcessed.PacketSize);
					}
					Packet->Close();
				}
				else
				{
					FPlatformProcess::Sleep(0.25f);
				}

				if (Socket == nullptr)
					break;
			}
		});
}


const int32 MaxPacketSize = 1024;
FString USTNetManager::ReceiveData()
{
	if (!Socket)
	{
		UE_LOG(LogTemp, Warning, TEXT("Socket Null Receive Not Fired!"));
		return "";
	}

	uint8 Buffer[MaxPacketSize];
	int32 BytesRead = 0;
	if (Socket->Recv(Buffer, sizeof(Buffer), BytesRead))
	{
		return FString(UTF8_TO_TCHAR((char*)Buffer));
	}

	return TEXT("");
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
