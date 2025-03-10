// Fill out your copyright notice in the Description page of Project Settings.


#include "STNetManager.h"
#include "Networking.h"
#include "Common/TcpSocketBuilder.h"
#include "STNet/Public/STNetSettings.h"
#include "Generated/GeneratedPacketHandler.h"

USTNetManager::USTNetManager()
	:UGameInstanceSubsystem()
{
}

USTNetManager::~USTNetManager()
{
	if(Connector)
		delete Connector;
	
	if(Listener)
		delete Listener;	
}

void USTNetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Socket = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("ClientSocket"), false);
	
	//Local Default
	FString ServerIP = "127.0.0.1";
	int32 ServerPort = 17777;
	if (const USTNetSettings* NetSetting = GetDefault<USTNetSettings>())
	{
		ServerIP = NetSetting->ServerIP;
		ServerPort = NetSetting->ServerPort;
	}

	PacketHandler = NewObject<USTNetPacketHandler>(this);
	PacketHandler->Initialize();
	bShouldStop = false;
	Socket->SetNonBlocking(true);

	Connector = new FNetThread(
		[this, ServerIP = MoveTemp(ServerIP), ServerPort]()
		{
			while (!bShouldStop)
			{
				if (Socket->GetConnectionState() != ESocketConnectionState::SCS_Connected)
				{
					if (ConnectToServer(ServerIP, ServerPort))
					{
						UE_LOG(LogTemp, Warning, TEXT("STNet Connected"));
						Listener = new FNetThread(
							[this]()
							{
								while (!bShouldStop &&
									Socket->GetConnectionState() == ESocketConnectionState::SCS_Connected)
								{
									StartReceiving();
								}
								UE_LOG(LogTemp, Warning, TEXT("Listener Exited Exited Successfuly"));
							});
						Listener->StartThread(TEXT("ClientListener"));
					}
				}				
				FPlatformProcess::Sleep(3.f);				
			}

			UE_LOG(LogTemp, Warning, TEXT("Connector Thread Exited Successfuly"));
		});
	Connector->StartThread(TEXT("ClientConnector"));

}

void USTNetManager::Deinitialize()
{
	Super::Deinitialize();

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("Deinitializing STNetManager"));
#endif

	bShouldStop = true;
	while (Connector->IsRunningThread() || Listener->IsRunningThread());
	CloseSocket();

#if WITH_EDITOR
	UE_LOG(LogTemp, Warning, TEXT("STNetManager DeInitialized"));
#endif
}


bool USTNetManager::ConnectToServer(const FString& ServerIP, int32 ServerPort)
{
	if (Socket == nullptr)
		return false;

	FIPv4Address IP;
	TSharedPtr<FInternetAddr> Addr;
	if (FIPv4Address::Parse(ServerIP, IP)==false)
	{
		//DDNS 동작
		ISocketSubsystem* SocketSubSystem = ISocketSubsystem::Get();
		
		FAddressInfoResult Result = SocketSubSystem->GetAddressInfo(*ServerIP, *FString::FromInt(ServerPort), EAddressInfoFlags::AllowV4MappedAddresses, "IPv4");
		for (FAddressInfoResultData Res : Result.Results)
		{
			if (Res.AddressProtocol == ESocketProtocolFamily::IPv4)
			{
				//UE_LOG(LogTemp, Warning, TEXT("DNS Address: %s"), *Res.Address->ToString(true));
				Addr = MoveTemp(Res.Address);
				break;
			}
		}
		// 서버 클라가 동일한 Local인 경우, 공유기가 NAT LoopBack없으면 안됨
		// 굳이 할라면 공유기 브릿지모드로...
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("DNS - %s 연결시도"), *Addr->ToString(true));	
#endif
		if (Socket->Connect(*Addr))
		{
			return true;
		}
		else
		{

#if WITH_EDITOR
			if (Result.Results.Num() == 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("DNS 못찾음 "));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("DNS - %s 연결불가. LocalHost로 시도"),*Addr->ToString(true));
			}
#endif


			Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
			FIPv4Address::Parse(TEXT("127.0.0.1"), IP);
			Addr->SetIp(IP.Value); // LocalHost
			Addr->SetPort(ServerPort);
			return Socket->Connect(*Addr);
		}
	}
	else
	{
		Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		Addr->SetIp(IP.Value);
		Addr->SetPort(ServerPort);
		return Socket->Connect(*Addr);
	}
}

void USTNetManager::CloseSocket()
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
	}
}


void USTNetManager::StartReceiving()
{	
	uint32 OutPendingData = 0;
	if (Socket->HasPendingData(OutPendingData))
	{
		TSharedPtr<FBufferArchive> Packet = ReceiveData();											
		if (Packet == nullptr)
			return;

		// 패킷 여러개 대비			
#if WITH_EDITOR
		UE_LOG(LogTemp, Warning, TEXT("from Server -%d byte"), Packet->TotalSize());
#endif
		uint32 ReadBytes = 0;
		while (ReadBytes < Packet->TotalSize())
		{
			PacketHeader Header;
			FMemory::Memcpy(&Header, Packet->GetData() + ReadBytes, sizeof(PacketHeader));
			ReadBytes += sizeof(PacketHeader);
			PacketType Type = static_cast<PacketType>(Header.PacketType);

#if WITH_EDITOR
			UE_LOG(LogTemp, Warning, TEXT("PacketType -%d"), Type);
			UE_LOG(LogTemp, Warning, TEXT("PacketSize -%d"), Header.PacketType);
#endif

			TArray<uint8> CopiedData;
			CopiedData.SetNumUninitialized(Header.PacketSize);
			FMemory::Memcpy(CopiedData.GetData(),Packet->GetData()+ ReadBytes, Header.PacketSize);

			AsyncTask(ENamedThreads::GameThread, [this, Type, MovedData = MoveTemp(CopiedData), Header]()
			{
				if (PacketHandler)
				{
					PacketHandler->DoJob(Type, MovedData, Header.PacketSize);
				}
			});
				
			ReadBytes += Header.PacketSize;
		}
		Packet->Close();
	}
	else
	{
		FPlatformProcess::Sleep(0.1f);
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
