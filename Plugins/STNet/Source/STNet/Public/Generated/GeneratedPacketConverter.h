
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GeneratedStructs.h"


inline void ToUnrealStruct(FAccountData* OutStruct, const AccountData& Data)
{
	OutStruct->ID = (UTF8_TO_TCHAR(Data.id().c_str()));
OutStruct->Password = (UTF8_TO_TCHAR(Data.password().c_str()));

}
inline void ToProtobuf(AccountData* OutProtobuf, const FAccountData& Data)
{
	OutProtobuf->set_id(TCHAR_TO_UTF8(*Data.ID));
OutProtobuf->set_password(TCHAR_TO_UTF8(*Data.Password));
	
}

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
	ToUnrealStruct(&OutStruct->AccountData,Data.accountdata());
OutStruct->IsAccountCreated = (Data.isaccountcreated());

}
inline void ToProtobuf(SC_Packet_Login* OutProtobuf, const FSC_Packet_Login& Data)
{
	ToProtobuf(OutProtobuf->mutable_accountdata(),Data.AccountData);
OutProtobuf->set_isaccountcreated(Data.IsAccountCreated);
	
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

inline void ToUnrealStruct(FDS_Packet_Dedi* OutStruct, const DS_Packet_Dedi& Data)
{
	OutStruct->BatcherID = (UTF8_TO_TCHAR(Data.batcherid().c_str()));
OutStruct->Port = (UTF8_TO_TCHAR(Data.port().c_str()));

}
inline void ToProtobuf(DS_Packet_Dedi* OutProtobuf, const FDS_Packet_Dedi& Data)
{
	OutProtobuf->set_batcherid(TCHAR_TO_UTF8(*Data.BatcherID));
OutProtobuf->set_port(TCHAR_TO_UTF8(*Data.Port));
	
}

inline void ToUnrealStruct(FSD_Packet_DediExit* OutStruct, const SD_Packet_DediExit& Data)
{
	OutStruct->BatcherID = (UTF8_TO_TCHAR(Data.batcherid().c_str()));
OutStruct->Port = (UTF8_TO_TCHAR(Data.port().c_str()));

}
inline void ToProtobuf(SD_Packet_DediExit* OutProtobuf, const FSD_Packet_DediExit& Data)
{
	OutProtobuf->set_batcherid(TCHAR_TO_UTF8(*Data.BatcherID));
OutProtobuf->set_port(TCHAR_TO_UTF8(*Data.Port));
	
}
