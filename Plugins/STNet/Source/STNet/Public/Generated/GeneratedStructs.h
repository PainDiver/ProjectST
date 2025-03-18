
#pragma once
#include "CoreMinimal.h"
#include "GeneratedProto.pb.h"
#include "GeneratedStructs.generated.h"


USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FAccountData
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString ID;
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Password;

};
//IsFromServer?DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_AccountData ,const FAccountData&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FCS_Packet_Login
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString UserId;
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Password;

};
//IsFromServer?DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_CS_Packet_Login ,const FCS_Packet_Login&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FSC_Packet_Login
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FAccountData AccountData;
UPROPERTY(BlueprintReadWrite, EditAnywhere) bool IsAccountCreated;

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_SC_Packet_Login ,const FSC_Packet_Login&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FCS_Packet_Match
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString UserId;

};
//IsFromServer?DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_CS_Packet_Match ,const FCS_Packet_Match&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FSC_Packet_Match
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString DedicateServerIP;

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_SC_Packet_Match ,const FSC_Packet_Match&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FDS_Packet_Dedi
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString BatcherID;
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Port;

};
//IsFromServer?DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_DS_Packet_Dedi ,const FDS_Packet_Dedi&, Message);



USTRUCT(BlueprintType,Blueprintable)
struct STNET_API FSD_Packet_DediExit
{
	GENERATED_BODY()
public:
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString BatcherID;
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString Port;

};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDele_SD_Packet_DediExit ,const FSD_Packet_DediExit&, Message);


