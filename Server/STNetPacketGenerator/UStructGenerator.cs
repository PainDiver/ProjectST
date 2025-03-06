using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static STPacketGenerator.Generator;


namespace STPacketGenerator
{
	public class Generator
	{

static List<PacketInfo> Packets = new List<PacketInfo>
{
		new PacketInfo("MyMessage", new string[] { "int32 ID;", "FString Name;" })
};

		public struct PacketInfo
		{
			public string PacketName;
			public string[] Vars;

			public PacketInfo(string packetName, string[] vars)
			{
				PacketName = packetName;
				Vars = vars;
			}
		}

		public static string GenerateHeader(string FileName)
		{
			string template = @"
#pragma once
#include ""CoreMinimal.h""
#include ""GeneratedProto.pb.h""
#include ""GenFile.generated.h""
";

			template  = template.Replace("GenFile", FileName);
			return template;
		}


		public static string GenerateUstruct(PacketInfo Packet)
		{
// 템플릿 정의
string template = @"
USTRUCT(BlueprintType,Blueprintable)
struct STNET_API F#PacketName
{
	GENERATED_BODY()
public:
	#Vars
};
";
			// #PacketName을 실제 값으로 교체
			template = template.Replace("#PacketName", Packet.PacketName);

			// UPROPERTY 자동화 및 #Vars 여러 속성값을 하나로 합쳐서 교체
			string allVars = string.Join("\n    ", AddUPROPERTY(Packet.Vars));  // UPROPERTY 추가된 속성들
			template = template.Replace("#Vars", allVars);

			return template;
		}

		public static string[] AddUPROPERTY(string[] Vars)
		{
			for (int i = 0; i < Vars.Length; i++)
			{
				Vars[i] = $"UPROPERTY(BlueprintReadWrite, EditAnywhere) {Vars[i]}";
			}
			return Vars;
		}

		public static void StartGenerating(string OutPutFilePath,string OutPutFileName,string FileType)
		{
			using (StreamWriter writer = new StreamWriter(OutPutFilePath + "\\" + OutPutFileName + FileType, false))  // false는 덮어쓰기를 의미
			{
				writer.WriteLine(GenerateHeader(OutPutFileName));

				foreach (var packet in Packets)
				{
					string result = GenerateUstruct(packet);
					writer.WriteLine(result);
					writer.WriteLine();
				}
			}
		}

	}
}
