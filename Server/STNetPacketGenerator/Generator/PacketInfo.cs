using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using STPacketGenerator.Generator;

namespace STPacketGenerator.Packet
{
	public enum ProtoPacketRealm
	{
		None,
		SC,
		CS
	}


	public struct PacketInfo
	{
		public ProtoPacketRealm Realm;
		public string PacketName;
		public PacketType PacketType;
		public List<PacketVar> Vars;

		public PacketInfo(string packetName, string[] vars, ProtoPacketRealm realm, PacketType packetType)
		{
			Vars = new List<PacketVar>();
			PacketName = packetName;
			foreach (string var in vars)
			{
				string[] typeName = var.Split(" ");
				Vars.Add(new PacketVar(typeName[0], typeName[1]));
			}
			Realm = realm;
			PacketType = packetType;
		}
		
	}
	public struct PacketVar
	{
		public string Type;
		public string VarName;

		public string[] InnerType;

		public PacketVar(string type, string name)
		{
			Type = type;
			VarName = name;
			InnerType = new string[2];

			if (type.StartsWith("TArray"))
			{
				Match match = Regex.Match(type, @"TArray<([^>]+)>");
				if (match.Success)
				{
					InnerType[0] = match.Groups[1].Value;
				}
			}
			else if (type.StartsWith("TMap"))
			{
				Match match = Regex.Match(type, @"TMap<([^,]+),\s*([^>]+)>");
				if (match.Success)
				{
					InnerType[0] = match.Groups[1].Value;
					InnerType[1] = match.Groups[2].Value;
				}
			}

		}



		private string CastTypeToUnreal(string Type, string Name)
		{
			if (Type == "FString")
			{
				return $"UTF8_TO_TCHAR({Name}.c_str())";
			}
			else
			{
				return Name;
			}
		}

		private string CastTypeToProtobuf(string Type, string Name)
		{
			if (Type == "FString")
			{
				return $"TCHAR_TO_UTF8(*{Name})";
			}
			else
			{
				return Name;
			}
		}

		private bool IsProtobufDefinedType(string type)
		{
			foreach (PacketInfo packet in Generator.Generator.Packets)
			{
				string substring = type.Substring(1);
				if (packet.PacketName == substring)
				{
					return true;
				}
			}
			return false;
		}

		public string ToUnrealStructLine(string unrealStructName,string protoBufName)
		{
			string protoBufVarName = $"{ protoBufName}.{VarName.ToLower()}()";
			string unrealStructVarName = $"{ unrealStructName }->{VarName}";
			
			if (Type.StartsWith("TArray"))
			{
				string nestedTemplate = @$"
	for (auto it = {protoBufVarName}.begin();it!={protoBufVarName}.end();++it)
	{{
		#protobufDefined
		{unrealStructVarName}.Add(#var);
	}}";

				string protobufDefined = "";
				string var = "";
				if (IsProtobufDefinedType(InnerType[0]))
				{
					protobufDefined = @$"
		{InnerType[0]} Var;
		ToUnrealStruct(&Var,*it);";
					var = "Var";
				}
				else
				{
					var = CastTypeToUnreal(InnerType[0], "*it");
				}
				nestedTemplate = nestedTemplate.Replace("#protobufDefined", protobufDefined);
				nestedTemplate = nestedTemplate.Replace("#var", var);
				return nestedTemplate;
			}
			else if (Type.StartsWith("TMap"))
			{
				string nestedTemplate = @$"
	for (auto it = {protoBufVarName}.begin(); it !=  {protoBufVarName}.end(); ++it)
	{{
		#protobufDefined0
		#protobufDefined1		
		{unrealStructVarName}.Add(#var0, #var1);
	}}";

				string[] protobufDefined = new string[2];
				string[] var = new string[2];
				for (int i = 0; i < 2; i++)
				{
					string property = i == 0 ? "first" : "second";
					if (IsProtobufDefinedType(InnerType[i]))
					{
		protobufDefined[i] = @$"
		{InnerType[i]} Var{i};
		ToUnrealStruct(&Var{i}, it->{property});";
		var[i] = $"Var{i}";
					}
					else
					{
						var[i] = CastTypeToUnreal(InnerType[i], $"it->{property}");
					}

					nestedTemplate = nestedTemplate.Replace($"#protobufDefined{i}", protobufDefined[i]);
					nestedTemplate = nestedTemplate.Replace($"#var{i}", var[i]);
				}
				
				return nestedTemplate;
			}
			else if (IsProtobufDefinedType(Type))
			{
				return $"ToUnrealStruct(&{unrealStructVarName},{protoBufVarName});\n";
			}
			else
			{
				return $"{unrealStructVarName} = ({CastTypeToUnreal(Type, protoBufVarName)});\n";
			}
		}




		public string ToProtobufLine(string unrealStructName, string protoBufName)
		{
			string protoSetBufVarName = $"{protoBufName}->set_{VarName.ToLower()}";
			string unrealStructVarName = $"{unrealStructName}.{VarName}";

			if (Type.StartsWith("TArray"))
			{
				string nestedTemplate = @$"
	for (auto it = {unrealStructVarName}.begin();it!={unrealStructVarName}.end();++it)
	{{
		#protobufDefined		
		#SetVar		
	}}";
				string var = "";
				string protobufDefined = "";
				if (IsProtobufDefinedType(InnerType[0]))
				{
					protobufDefined = @$"
		{InnerType[0].Substring(1)}* Var = {protoBufName}->add_{VarName.ToLower()}();
		ToProtobuf(Var,*it);";
					var = "Var";
					nestedTemplate = nestedTemplate.Replace("#SetVar", "");
				}
				else
				{
					nestedTemplate = nestedTemplate.Replace("#SetVar", $"{protoBufName}->add_{VarName.ToLower()}(#var);");
					var = CastTypeToProtobuf(InnerType[0], "*it");
				}

				nestedTemplate = nestedTemplate.Replace("#protobufDefined", protobufDefined);
				nestedTemplate = nestedTemplate.Replace("#var", var);
				return nestedTemplate;
			}
			else if (Type.StartsWith("TMap"))
			{
				string nestedTemplate = @$"
	for (auto it = {unrealStructVarName}.begin(); it !=  {unrealStructVarName}.end(); ++it)
	{{
		#protobufDefined0
		#protobufDefined1		
		{protoBufName}->mutable_{VarName.ToLower()}()->insert({{#var0, #var1}});
	}}";

				string[] protobufDefined = new string[2];
				string[] var = new string[2];
				for (int i = 0; i < 2; i++)
				{
					string property = i == 0 ? "Key" : "Value";
					if (IsProtobufDefinedType(InnerType[i]))
					{
		protobufDefined[i] = @$"
		{InnerType[i].Substring(1)} Var{i};
		ToProtobuf(&Var{i}, it->{property});";
						var[i] = $"Var{i}";
					}
					else
					{
						var[i] = CastTypeToProtobuf(InnerType[i], $"it->{property}");
					}

					nestedTemplate = nestedTemplate.Replace($"#protobufDefined{i}", protobufDefined[i]);
					nestedTemplate = nestedTemplate.Replace($"#var{i}", var[i]);
				}

				return nestedTemplate;
			}
			else if (IsProtobufDefinedType(Type))
			{
				return $"ToProtobuf({protoBufName}->mutable_{VarName.ToLower()}(),{unrealStructVarName});\n";
			}
			else
			{
				return $"{protoSetBufVarName}({CastTypeToProtobuf(Type, unrealStructVarName)});\n";
			}

		}

	}

	
}
