using Google.Protobuf;
using Grpc.Net.Client;
using MongoDB.Bson.Serialization;
using STNet_GameInstanceBatcher_Client;
using STNetUtils.RESTContext;
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
		List<DedicateServerBatcher> ConnectedBatcher;

		private GameInstanceManager() 
		{
			MatchingQueue = new ConcurrentQueue<MatchingClientInfo>();
			ServerInfo = new List<DedicateServerInfo>();
			MatchingQueueLock = new object();
			DedicateServerInfoLock = new object();
			ConnectedBatcher = new List<DedicateServerBatcher>();
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
				ConnectedBatcher.Add(batcher);

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
				RunAnyDedicateServer();
			}
		}

		
		public void RunAnyDedicateServer()
		{
			foreach (DedicateServerBatcher batcher in ConnectedBatcher)
			{
				foreach (DedicateServerInfo dedicateServerInfo in batcher.DedicateServersInCharge)
				{
					if (dedicateServerInfo.ServerState == DedicateServerState.Dead)
					{
						RunDedicateServer(batcher,dedicateServerInfo);
					}
				}
			}
		}



		void RunDedicateServer(DedicateServerBatcher batcher, DedicateServerInfo dedicateServerInfo)
		{
			dedicateServerInfo.ServerState = DedicateServerState.Pending;
			DedicateServerParam param = new DedicateServerParam();
			param.BatcherID = batcher.Id;
			param.Port = dedicateServerInfo.Port;
			batcher.ConnectedRPC.RunDedicateServer(param);
		}


		public void MarkDedicateServerAlive(string batcherID, string port)
		{
			foreach (DedicateServerBatcher batcher in ConnectedBatcher)
			{
				if (batcherID == batcher.Id)
				{
					foreach (DedicateServerInfo dediInfo in batcher.DedicateServersInCharge)
					{
						if(dediInfo.Port == port)
						{
							dediInfo.ServerState = DedicateServerState.Alive;
							return;
						}
					}
				}
			}
		}

		public void EndDedicateServer(string port)
		{
			foreach (DedicateServerBatcher batcher in ConnectedBatcher)
			{
				foreach (DedicateServerInfo dedicateServerInfo in batcher.DedicateServersInCharge)
				{
					if (dedicateServerInfo.ServerState == DedicateServerState.Alive && dedicateServerInfo.Port == port)
					{
						EndDedicateServer(batcher,dedicateServerInfo);
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

			Socket dedicateSocket = ServerCore.Instance.GetDedicateServer(param.Port);

			SD_Packet_DediExit packet = new SD_Packet_DediExit();
			packet.BatcherID = batcher.Id;
			packet.Port = dedicateServerInfo.Port;
			ServerCore.Instance.Send(dedicateSocket, PacketType.PtSdDediexit, packet.ToByteArray(), packet.CalculateSize());
		}

	}
}
