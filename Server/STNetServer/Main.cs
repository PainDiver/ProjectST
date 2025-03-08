using System;
using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using MongoDB.Driver;
using STNetServer.Core;


class TcpServer
{
	static async Task Main()
	{
		string DBDirectory = Directory.GetCurrentDirectory() + "\\..\\..\\..\\..\\DB";
		DBCore DB = new DBCore(DBDirectory);
		DB.StartDB();

		ServerCore Server = new ServerCore(17777);
		Server.StartServer(DB);

		Console.WriteLine("서버 명령어 입력");
		while (Server.IsServerActive)
		{
			string command = await Task.Run(() => Console.ReadLine());
			Server.ReadCommand(command);
		}
	}
}