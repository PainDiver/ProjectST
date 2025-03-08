using Microsoft.VisualBasic.FileIO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using static STPacketGenerator.Generator.Generator;
using Google.Protobuf.Reflection;
using static System.Runtime.InteropServices.JavaScript.JSType;
using STPacketGenerator.Packet;
using System.Runtime.CompilerServices;
using System.Reflection.PortableExecutable;


namespace STPacketGenerator.Generator
{
	using PPR = ProtoPacketRealm;
	using PT = PacketType;
	public class Generator
	{
		public string OutputFilePath;

		public Generator(string outputFilePath)
		{
			OutputFilePath = outputFilePath;
		}

		static public List<PacketInfo> Packets = new List<PacketInfo>
		{
			new PacketInfo("CS_Packet_Login", new string[] { "FString UserId", "FString Password"},PPR.CS,PT.PtCsLogin),
			new PacketInfo("SC_Packet_Login", new string[] { "FString UserId" },PPR.SC,PT.PtScLogin),
			new PacketInfo("CS_Packet_Match", new string[] { "FString UserId" },PPR.CS,PT.PtCsMatch),
			new PacketInfo("SC_Packet_Match", new string[] { "FString DedicateServerIP" },PPR.SC,PT.PtScMatch)
		};


		public void GenerateStructs(string outPutFileName, string fileType)
		{
			using (StreamWriter writer = new StreamWriter(OutputFilePath + "\\" + outPutFileName + fileType, false))  // false는 덮어쓰기를 의미
			{
				string template = @"
#pragma once
#include ""CoreMinimal.h""
#include ""GeneratedProto.pb.h""
#include ""GenFile.generated.h""
";
				template = template.Replace("GenFile", outPutFileName);
				writer.WriteLine(template);

				foreach (var packet in Packets)
				{
					string structTemplate = @"
USTRUCT(BlueprintType,Blueprintable)
struct STNET_API F#PacketName
{
	GENERATED_BODY()
public:
#Vars
};
//IsFromServer?DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(#DeleagteName ,const F#PacketName&, Message);
";

					if (packet.Realm == ProtoPacketRealm.SC)
					{
						structTemplate = structTemplate.Replace("//IsFromServer?", "");
					}
					structTemplate = structTemplate.Replace("#DeleagteName", "FDele_" + packet.PacketName);
					// #PacketName을 실제 값으로 교체
					structTemplate = structTemplate.Replace("#PacketName", packet.PacketName);

					// UPROPERTY 자동화 및 #Vars 여러 속성값을 하나로 합쳐서 교체
					string allVars = "";
					foreach (PacketVar var in packet.Vars)
					{
						allVars += $"UPROPERTY(BlueprintReadWrite, EditAnywhere) {var.Type} {var.VarName};\n";
					}
			;  // UPROPERTY 추가된 속성들
					structTemplate = structTemplate.Replace("#Vars", allVars);

					writer.WriteLine(structTemplate);
					writer.WriteLine();
				}
			}
		}

		public void GeneratePacketHandler(string outPutFileName, string fileType)
		{
			string template = @"
#pragma once
#include ""CoreMinimal.h""
#include ""UObject/NoExportTypes.h""
#include ""GeneratedStructs.h""
#include ""STNetManager.h""
#include ""GeneratedPacketConverter.h""
#include ""#outPutFileName.generated.h""

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
		#CallRecvFunc
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

#CallSendFunc

#vars;
};
";

			string callRecvFunc = "";
			string callSendFunc = "";
			foreach (PacketInfo packet in Packets)
			{
				if (packet.Realm == PPR.SC)
				{
					var fieldInfo = typeof(PacketType).GetField(packet.PacketType.ToString());
					var attribute = fieldInfo?.GetCustomAttribute<OriginalNameAttribute>();
					string initFuncTemplate = @"
		Delegates.Add(PacketType::#PT, 
		[this](uint8 * Data, uint16 PacketSize)
		{
			F#PacketName Message;
			FMemory::Memmove(&Message, Data, PacketSize);
			Dele_#PacketName.Broadcast(Message); 
		});";
					initFuncTemplate = initFuncTemplate.Replace("#PT", attribute.Name);
					initFuncTemplate = initFuncTemplate.Replace("#PacketName", packet.PacketName);
					callRecvFunc += initFuncTemplate;
				}

				if (packet.Realm == PPR.CS)
				{
					var fieldInfo = typeof(PacketType).GetField(packet.PacketType.ToString());
					var attribute = fieldInfo?.GetCustomAttribute<OriginalNameAttribute>();
					string callFuncTemplate = @"
	UFUNCTION(BlueprintCallable)
	bool SendData_F#PacketName(const F#PacketName& Data)
	{		
		#PacketName Packet;
		ToProtobuf(&Packet, Data);
		PacketHeader Header;
		Header.PacketType = #PT;
		Header.PacketSize = Packet.ByteSizeLong();
		TArray<uint8> Buffer;
		Buffer.SetNumUninitialized(sizeof(PacketHeader) + Packet.ByteSizeLong());
		FMemory::Memcpy(Buffer.GetData(),&Header,sizeof(PacketHeader));		
		Packet.SerializeToArray(Buffer.GetData()+sizeof(PacketHeader), Packet.ByteSizeLong());
		return USTNetManager::Get()->SendData_Internal(Buffer.GetData(), sizeof(PacketHeader)+Packet.ByteSizeLong());
	};";

					callFuncTemplate = callFuncTemplate.Replace("#PT", attribute.Name);
					callFuncTemplate = callFuncTemplate.Replace("#PacketName", packet.PacketName);
					callSendFunc += callFuncTemplate;
				}
			}

			template = template.Replace("#CallRecvFunc", callRecvFunc);
			template = template.Replace("#CallSendFunc", callSendFunc);


			template = template.Replace("#outPutFileName", outPutFileName);
			string DelegatesLine = "";
			foreach (PacketInfo packet in Packets)
			{
				if (packet.Realm == PPR.SC)
				{
					DelegatesLine += "\nUPROPERTY(BlueprintAssignable)\n" + $"FDele_{packet.PacketName} Dele_{packet.PacketName};\n";
				}
			}

			template = template.Replace("#vars", DelegatesLine);
			using (StreamWriter writer = new StreamWriter(OutputFilePath + "\\" + outPutFileName + fileType, false))  // false는 덮어쓰기를 의미
			{
				writer.WriteLine(template);
			}
		}



		public void GeneratePacketConverter(string outPutFileName, string fileType)
		{
			using (StreamWriter writer = new StreamWriter(OutputFilePath + "\\" + outPutFileName + fileType, false))  // false는 덮어쓰기를 의미
			{
				string headerTemplate = @"
#pragma once
#include ""CoreMinimal.h""
#include ""UObject/NoExportTypes.h""
#include ""GeneratedStructs.h""
";
				writer.WriteLine(headerTemplate);

				foreach (PacketInfo packet in Packets)
				{
					string functionTemplate = @"
inline void ToUnrealStruct(F#PacketName* OutStruct, const #PacketName& Data)
{
	#SetUnrealVars
}
inline void ToProtobuf(#PacketName* OutProtobuf, const F#PacketName& Data)
{
	#SetProtobufVars	
}";

					string SetUnrealVarsLine = "";
					string SetProtobufVarsLine = "";
					foreach (PacketVar var in packet.Vars)
					{
						SetUnrealVarsLine += var.ToUnrealStructLine("OutStruct", "Data");
						SetProtobufVarsLine += var.ToProtobufLine("Data","OutProtobuf");
					}

					functionTemplate = functionTemplate.Replace("#PacketName", packet.PacketName);
					functionTemplate =  functionTemplate.Replace("#SetProtobufVars", SetProtobufVarsLine);

					functionTemplate = functionTemplate.Replace("#SetUnrealVars", SetUnrealVarsLine);
					functionTemplate = functionTemplate.Replace("#SetProtobufVars", SetProtobufVarsLine);


					writer.WriteLine(functionTemplate);
					
				}
			}
		}
	}
}
