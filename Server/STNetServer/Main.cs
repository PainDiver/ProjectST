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

public class GlobalVariables
{
	public static ServerConfig? Config;
}

class TcpServer
{
	static async Task Main()
	{
		AppDomain.CurrentDomain.ProcessExit += new EventHandler(OnProcessExit);

		string json = File.ReadAllText("..\\..\\..\\Json\\ServerConfig.json");
		GlobalVariables.Config = JsonConvert.DeserializeObject<ServerConfig>(json);

		DBCore.Instance.StartDBServer();
		ServerCore.Instance.StartServer();

		Console.WriteLine("서버 명령어 입력");
		while (ServerCore.Instance.IsServerActive)
		{
			string command = await Task.Run(() => Console.ReadLine());
			ServerCore.Instance.ReadCommand(command);
		}
	}

	static void OnProcessExit(object sender, EventArgs e)
	{
		if (ServerCore.Instance != null)
		{
			ServerCore.Instance.EndSocket();
		}

		if (DBCore.Instance != null)
		{
			DBCore.Instance.EndDB();
		}
	}
}