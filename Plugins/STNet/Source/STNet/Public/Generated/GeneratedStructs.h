
#pragma once
#include "CoreMinimal.h"
#include "GeneratedProto.pb.h"
#include "GeneratedStructs.generated.h"


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
UPROPERTY(BlueprintReadWrite, EditAnywhere) FString UserId;

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


