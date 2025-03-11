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
using STNetServer.Mail;

namespace STNetServer.Global
{
	public class GlobalConfig
	{
		public static ServerConfig? Config;
		public static MailConfig? MailConfig;

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
		if (File.Exists("Json\\ServerConfig.json"))
		{
			string Serverjson = File.ReadAllText("Json\\ServerConfig.json");
			GlobalConfig.Config = JsonConvert.DeserializeObject<ServerConfig>(Serverjson);
		}
		
		if (GlobalConfig.Config == null)
		{
			Console.WriteLine("Config 초기화되지 않음!");
		}

		if (File.Exists("Json\\MailCredentials.json"))
		{
			string mailJson = File.ReadAllText("Json\\MailCredentials.json");
			GlobalConfig.MailConfig = JsonConvert.DeserializeObject<MailConfig>(mailJson);
		}

		if (GlobalConfig.MailConfig == null)
		{
			Console.WriteLine("MailConfig 초기화되지 않음!");
		}


		DBCore.Instance.StartDBServer();

		// Thread Max 12
		int maxConnection = 100;
		string serverPort = GlobalConfig.Config.ServerPort;
		int jobWorker = 6;
		ServerCore.Instance.StartServer(maxConnection, serverPort, jobWorker);


		if (GlobalConfig.MailConfig != null)
		{
			SMTPSender.SendSMTP(SMTPServerType.naver,new NetworkCredential(
				GlobalConfig.MailConfig.AlerterEmail, 
				GlobalConfig.MailConfig.AlerterPassword),
				GlobalConfig.MailConfig.ReceiverMail,
				"[SMTP] C# 서버 구동!",
				$"C# 서버 구동에 성공했습니다. DNS :{GlobalConfig.Config.ServerDNS} Port : {GlobalConfig.Config.ServerPort}");
			Console.WriteLine("서버 구동 이메일 알람을 보냈습니다!");
		}
		else
		{
			Console.WriteLine("서버 구동 이메일 알람을 스킵합니다!");
		}

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