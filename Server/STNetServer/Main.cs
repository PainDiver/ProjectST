using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using MongoDB.Driver;
using STNetServer.Core;
using STNetServer.Json;
using Newtonsoft.Json;
using STNetServer.Core.DB;
using STNetServer.Core.Utils;
using STNetServer.Global;

namespace STNetServer.Global
{
	public class GlobalConfig
	{
		public static ServerConfig? Config;
	}

	public class GlobalFunction
	{
		public static Process RunProgram(string Path, string Args)
		{
			
			Process process = new Process();
			Console.WriteLine($"Running Program : {Path}");
			process.StartInfo.FileName = Path;  // protoc 실행 파일
			process.StartInfo.Arguments = Args;
			process.StartInfo.UseShellExecute = true;
			process.StartInfo.Verb = "runas";
			process.Start();

			return process;
		}
	}
}

class TcpServer
{
	static async Task Main()
	{
		AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);

		ThreadPool.GetMinThreads(out int maxWorkerThreads, out int iocpThreads);
#if DEBUG
		Console.WriteLine($"최대 워커쓰레드 수: {maxWorkerThreads}");
#endif

		string json = File.ReadAllText("Json\\ServerConfig.json");
		GlobalConfig.Config = JsonConvert.DeserializeObject<ServerConfig>(json);
		if (GlobalConfig.Config == null)
		{
			Console.WriteLine("Config 초기화되지 않음!");
		}

		DBCore.Instance.StartDBServer();

		// Thread Max 12
		int maxConnection = 100;
		string serverPort = GlobalConfig.Config.ServerPort;
		int jobWorker = 6;
		ServerCore.Instance.StartServer(maxConnection, serverPort, jobWorker);

		Console.WriteLine("서버 명령어 입력");
		while (ServerCore.Instance.CancelToken.IsCancellationRequested == false)
		{
			string command = await Task.Run(() => Console.ReadLine());
			ServerCore.Instance.ReadCommand(command);
		}
	}

	static void OnProcessExit(object sender, EventArgs e)
	{
		if (ServerCore.Instance != null)
		{
			ServerCore.Instance.CloseServer();
		}
	}

}