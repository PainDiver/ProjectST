using System;
using System.Diagnostics;
using System.IO;
using STPacketGenerator;


class ProtoGenerator
{
	static void Main()
	{
		string protoDir = Directory.GetCurrentDirectory() + "\\..\\..\\..";  // .proto 파일 저장 위치
		string outCpp = Directory.GetCurrentDirectory() + "\\..\\..\\..\\..\\..\\Plugins\\STNet\\Source\\STNet\\Public";  // C++ 코드 출력 위치
		string outCSharp = Directory.GetCurrentDirectory() + "\\..\\..\\..\\..\\STNetServer\\Generated";  // C# 코드 출력 위치

		// protoc 실행: C++ 코드 생성
		if (RunProtoc($"--proto_path={protoDir} --cpp_out={outCpp} {protoDir}/GeneratedProto.proto"))
		{
			Generator.StartGenerating(outCpp,"GeneratedStructs",".h");

			Console.WriteLine("C++ Protobuf 코드 자동 생성 완료!");
		}
		else
		{
			Console.WriteLine("C++ Protobuf 코드 자동 생성 실패!");
		}

		// protoc 실행: C# 코드 생성
		if (RunProtoc($"--proto_path={protoDir} --csharp_out={outCSharp} {protoDir}/GeneratedProto.proto"))
		{
			Console.WriteLine("C# Protobuf 코드 자동 생성 완료!");
		}
		else
		{
			Console.WriteLine("C# Protobuf 코드 자동 생성 실패!");
		}
	}

	static bool RunProtoc(string args)
	{

		Process process = new Process();
		process.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\..\\..\\..\\protoc\\protoc";  // protoc 실행 파일
		process.StartInfo.Arguments = args;
		process.StartInfo.UseShellExecute = false;
		process.StartInfo.RedirectStandardOutput = true;
		process.Start();
		process.WaitForExit();
		return true;
	}
}