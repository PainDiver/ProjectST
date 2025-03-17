using STNet_GameInstanceBatcher_Server;
using System.Diagnostics;
using System.Net;

namespace STNetGameInstanceBatcher
{
	public class DedicateServerInstanceManager
	{
		private static Lazy<DedicateServerInstanceManager> LazyInstance = new Lazy<DedicateServerInstanceManager>(() => new DedicateServerInstanceManager());
		public static DedicateServerInstanceManager Instance = LazyInstance.Value;

		//port,Process
		private Dictionary<string, Process> DedicateServers;

		private DedicateServerInstanceManager() 
		{
			DedicateServers = new Dictionary<string, Process>();
		}

		public DedicateServerInfo RunDedicateServerInstance(string batcherID,string port)
		{
			Process DedicateServer = new Process();
			DedicateServer.StartInfo.Arguments = $"-log -port={port} -STBatcherID={batcherID}";
			DedicateServer.StartInfo.FileName = "DedicateServer/WindowsServer/ProjectSTServer";
			DedicateServer.StartInfo.Verb = "runas";
			DedicateServerInfo server = new DedicateServerInfo();
			if (DedicateServer.Start())
			{
				// 데디 실행할때 MainServer로 보내면 그때 Alive로 될 예정
				server.ServerState = DedicateServerState.Pending;
				string localIP = string.Empty;
				foreach (var ip in Dns.GetHostAddresses(Dns.GetHostName()))
				{
					if (ip.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork)
					{
						server.IP = ip.ToString();
						break;
					}
				}
				server.Port = port;
				DedicateServers.Add(port, DedicateServer);

			}
			else
			{
				server.ServerState = DedicateServerState.Dead;
			}
			return server;
		}


	}
}
