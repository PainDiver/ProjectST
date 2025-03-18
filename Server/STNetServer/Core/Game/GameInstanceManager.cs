using Amazon.Runtime.Internal.Transform;
using Google.Protobuf;
using Grpc.Net.Client;
using MongoDB.Bson.Serialization;
using STNet_GameInstanceBatcher_Client;
using System;
using System.Collections.Concurrent;
using System.Diagnostics;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using static STNet_GameInstanceBatcher_Client.Match;


namespace STNetServer.Core.NewFolder
{

	public class DedicateServerBatcher
	{
		public DedicateServerBatcher(string id,MatchClient matchClient) 
		{
			Id = id;
			ConnectedRPC = matchClient;
		}

		public string Id;
		public MatchClient ConnectedRPC;
		public List<DedicateServerInfo> DedicateServersInCharge;
	}

	public class MatchingClientInfo
	{
		public MatchingClientInfo(string name)
		{
			ClientID = name;
		}
		public string? ClientID;
	}
	public class GameInstanceInfo
	{
		public GameInstanceInfo(string batcherID, string port,List<MatchingClientInfo> clients)
		{
			BatcherID = batcherID;
			Port = port;
			Clients = clients;
		}
		public string? BatcherID;
		public string? Port;
		public List<MatchingClientInfo> Clients;
	}


	public class GameInstanceManager
	{
		private static Lazy<GameInstanceManager> LazyQueueManager = new Lazy<GameInstanceManager>(() => new GameInstanceManager());

		public static GameInstanceManager Instance = LazyQueueManager.Value;

		private ConcurrentQueue<MatchingClientInfo> MatchingQueue;

		private int MatchingQueueNum;
		private object MatchingQueueLock = new object();
		private object DedicateServerInfoLock = new object();

		// 데디 통신용 비동기 이벤트
		List<DedicateServerInfo> ServerInfo;
		Dictionary<string,DedicateServerBatcher> ConnectedBatcher;
		List<GameInstanceInfo> PendingMatchedServer;

		private GameInstanceManager() 
		{
			MatchingQueue = new ConcurrentQueue<MatchingClientInfo>();
			ServerInfo = new List<DedicateServerInfo>();
			MatchingQueueLock = new object();
			DedicateServerInfoLock = new object();
			ConnectedBatcher = new Dictionary<string, DedicateServerBatcher>();
			PendingMatchedServer = new List<GameInstanceInfo>();
		}

		public void Initialize(int matchingQueueNum, List<string> gameInstanceServerIps, int dedicateServerCountPerBatcher)
		{
			MatchingQueueNum = matchingQueueNum;
			for (int i = 0; i < dedicateServerCountPerBatcher; i++)
			{
				DedicateServerInfo dedicateServerInfo = new DedicateServerInfo();
				dedicateServerInfo.ServerState = DedicateServerState.Dead;
				dedicateServerInfo.IP = string.Empty;
				dedicateServerInfo.Port = $"{7777 + i}";
				ServerInfo.Add(dedicateServerInfo);
			}

			int DediPerServerCount = ServerInfo.Count / gameInstanceServerIps.Count;		
			for (int i = 0; i < gameInstanceServerIps.Count; i++)
			{
				GrpcChannel newChannel = GrpcChannel.ForAddress(gameInstanceServerIps[i]);
				DedicateServerBatcher batcher = new DedicateServerBatcher(i.ToString(),new Match.MatchClient(newChannel));
				int Remains = 0;
				if (i == gameInstanceServerIps.Count - 1)
				{
					Remains = ServerInfo.Count - (i * DediPerServerCount + DediPerServerCount);
				}
				batcher.DedicateServersInCharge = ServerInfo.GetRange(i * DediPerServerCount, DediPerServerCount+Remains);
				ConnectedBatcher.Add(i.ToString(),batcher);

				batcher.ConnectedRPC.OnConnectInitially(new RPCVoid());
			}
		}

		public void EnqueueClientToMatchingServer(MatchingClientInfo client)
		{
			MatchingQueue.Enqueue(client);
			List<MatchingClientInfo> clients = null;

			bool bShouldStartDedicateServer = false;
			lock (MatchingQueueLock)
			{
				if (MatchingQueueNum == MatchingQueue.Count)
				{
					clients = MatchingQueue.ToList();
					MatchingQueue.Clear();
					bShouldStartDedicateServer = true;
				}
			}

			if (bShouldStartDedicateServer)
			{
				RunAnyDedicateServer(out string batcherID, out string port);
				PendingMatchedServer.Add(new GameInstanceInfo(batcherID,port,clients));
			}
		}

		
		public void RunAnyDedicateServer(out string batcherID, out string port)
		{
			batcherID = string.Empty;
			port = string.Empty;
			foreach (var batcher in ConnectedBatcher)
			{
				foreach (DedicateServerInfo dedicateServerInfo in batcher.Value.DedicateServersInCharge)
				{
					if (dedicateServerInfo.ServerState == DedicateServerState.Dead)
					{
						batcherID = batcher.Value.Id;
						port = dedicateServerInfo.Port;
						RunDedicateServer(batcher.Value,dedicateServerInfo);
						break;
					}
				}
			}
		}



		async Task RunDedicateServer(DedicateServerBatcher batcher, DedicateServerInfo dedicateServerInfo)
		{
			dedicateServerInfo.ServerState = DedicateServerState.Pending;
			DedicateServerParam param = new DedicateServerParam();
			param.BatcherID = batcher.Id;
			param.Port = dedicateServerInfo.Port;
			DedicateServerInfo info = await batcher.ConnectedRPC.RunDedicateServerAsync(param);

			if (ConnectedBatcher.ContainsKey(batcher.Id))
			{
				var elem = ConnectedBatcher[batcher.Id].DedicateServersInCharge.Find(elem => elem.Port == info.Port);
				if(elem != null)
					elem.IP = info.IP;
			}

		}

		public DedicateServerBatcher GetConnectedBatcher(string batcherID)
		{
			if (ConnectedBatcher.ContainsKey(batcherID))
			{
				return ConnectedBatcher[batcherID];
			}
			return null;
		}

		public void OnDedicateServerLive(string batcherID, string port)
		{			
			if (ConnectedBatcher.ContainsKey(batcherID))
			{
				foreach (DedicateServerInfo dediInfo in ConnectedBatcher[batcherID].DedicateServersInCharge)
				{
					if(dediInfo.Port == port)
					{
						dediInfo.ServerState = DedicateServerState.Alive;

						GameInstanceInfo? gameInstance = PendingMatchedServer.Find((GameInstanceInfo info) => info.BatcherID == batcherID);
						if (gameInstance != null)
						{
							foreach (MatchingClientInfo client in gameInstance.Clients)
							{
								Socket clientSocket = ServerCore.Instance.GetClient(client.ClientID);

								SC_Packet_Match Match;
								Match = new SC_Packet_Match();
								Match.DedicateServerIP = dediInfo.IP;
								Match.Port = port;
								ServerCore.Instance.SendPacket<SC_Packet_Match>(clientSocket,PacketType.PtScMatch,Match);
							}
							PendingMatchedServer.Remove(gameInstance);
						}
						return;
					}
				}
			}
			
		}

		public void EndDedicateServer(string port)
		{
			foreach (var batcher in ConnectedBatcher)
			{
				foreach (DedicateServerInfo dedicateServerInfo in batcher.Value.DedicateServersInCharge)
				{
					if (dedicateServerInfo.ServerState == DedicateServerState.Alive && dedicateServerInfo.Port == port)
					{
						EndDedicateServer(batcher.Value,dedicateServerInfo);
					}
				}
			}
		}

		void EndDedicateServer(DedicateServerBatcher batcher, DedicateServerInfo dedicateServerInfo)
		{
			DedicateServerParam param = new DedicateServerParam();
			param.BatcherID = batcher.Id;
			param.Port = dedicateServerInfo.Port;
			dedicateServerInfo.ServerState = DedicateServerState.Dead;

			ConnectedDedicateServerInfo info = ServerCore.Instance.GetDedicateServer(param.Port);

			SD_Packet_DediExit packet = new SD_Packet_DediExit();
			packet.BatcherID = batcher.Id;
			packet.Port = dedicateServerInfo.Port;
			ServerCore.Instance.SendPacket<SD_Packet_DediExit>(info.Socket, PacketType.PtSdDediexit,packet);
		}

	}
}
