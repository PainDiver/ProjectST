
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GeneratedStructs.h"


inline void ToUnrealStruct(FCS_Packet_Login* OutStruct, const CS_Packet_Login& Data)
{
	OutStruct->UserId = (UTF8_TO_TCHAR(Data.userid().c_str()));
OutStruct->Password = (UTF8_TO_TCHAR(Data.password().c_str()));

}
inline void ToProtobuf(CS_Packet_Login* OutProtobuf, const FCS_Packet_Login& Data)
{
	OutProtobuf->set_userid(TCHAR_TO_UTF8(*Data.UserId));
OutProtobuf->set_password(TCHAR_TO_UTF8(*Data.Password));
	
}

inline void ToUnrealStruct(FSC_Packet_Login* OutStruct, const SC_Packet_Login& Data)
{
	OutStruct->UserId = (UTF8_TO_TCHAR(Data.userid().c_str()));

}
inline void ToProtobuf(SC_Packet_Login* OutProtobuf, const FSC_Packet_Login& Data)
{
	OutProtobuf->set_userid(TCHAR_TO_UTF8(*Data.UserId));
	
}

inline void ToUnrealStruct(FCS_Packet_Match* OutStruct, const CS_Packet_Match& Data)
{
	OutStruct->UserId = (UTF8_TO_TCHAR(Data.userid().c_str()));

}
inline void ToProtobuf(CS_Packet_Match* OutProtobuf, const FCS_Packet_Match& Data)
{
	OutProtobuf->set_userid(TCHAR_TO_UTF8(*Data.UserId));
	
}

inline void ToUnrealStruct(FSC_Packet_Match* OutStruct, const SC_Packet_Match& Data)
{
	OutStruct->DedicateServerIP = (UTF8_TO_TCHAR(Data.dedicateserverip().c_str()));

}
inline void ToProtobuf(SC_Packet_Match* OutProtobuf, const FSC_Packet_Match& Data)
{
	OutProtobuf->set_dedicateserverip(TCHAR_TO_UTF8(*Data.DedicateServerIP));
	
}
