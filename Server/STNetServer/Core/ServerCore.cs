using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using Pipelines.Sockets.Unofficial.Buffers;
using System.Buffers;
using Google.Protobuf;
using Microsoft.VisualBasic;
using MongoDB.Bson.Serialization.Serializers;
using STNetServer.Core.Jobs;
using System.Threading;
using STNetServer.Core.Utils;
using STNetServer.Core.DB;
using STNetServer.Core.NewFolder;
using System.IO;
using System.Drawing;
using static System.Runtime.InteropServices.JavaScript.JSType;
using Amazon.Runtime.Internal.Transform;

namespace STNetServer.Core
{

	public struct ConnectedDedicateServerInfo
	{
		public ConnectedDedicateServerInfo(Socket socket,string ip, string port)
		{
			Socket = socket;
			IP = ip;
			Port = port;
		}

		public Socket Socket;
		public string IP;
		public string Port;
	}

	class ServerCore
	{
		private static readonly Lazy<ServerCore> ServerCoreInstance = new Lazy<ServerCore>(() => new ServerCore());

		public static ServerCore Instance => ServerCoreInstance.Value;

		private static CancellationTokenSource CancelTokenSource = new CancellationTokenSource();
		public CancellationToken CancelToken;

		private Socket ListenSocket;
		Dictionary<int, SocketAsyncEventArgs> ConnectedEvents;

		// id - socket
		Dictionary<string, Socket> ConnectedClients;
		public Socket GetClient(string id)
		{
			Socket socket;
			ConnectedClients.TryGetValue(id, out socket);
			return socket;
		}

		// port - socket
		Dictionary<string, ConnectedDedicateServerInfo> ConnectedDedicateServer;
		
		public ConnectedDedicateServerInfo GetDedicateServer(string port)
		{
			ConnectedDedicateServerInfo Info;
			ConnectedDedicateServer.TryGetValue(port,out Info);
			return Info;
		}


		PacketHandler? PacketHandler;

		public int MaxConnections = 100;
		public int MaxPacektSize = 1024;

		private ServerCore()
		{
			ListenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			ConnectedClients = new Dictionary<string,Socket>();
			ConnectedDedicateServer = new Dictionary<string,ConnectedDedicateServerInfo>();
			ConnectedEvents = new Dictionary<int, SocketAsyncEventArgs>();
			CancelToken = CancelTokenSource.Token;
		}

		public void StartServer(int MaxConnection, string Port, int workerThreadCount)
		{
			IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, int.Parse(Port));
			ListenSocket.Bind(localEndPoint);
			ListenSocket.Listen(MaxConnections);
			Console.WriteLine($"서버가 {Port} 포트에서 시작 됨");
			// 클라이언트 연결을 기다리며 비동기 작업 시작
			PacketHandler = new PacketHandler(workerThreadCount);
			StartAccept(null);
		}

		private void StartAccept(SocketAsyncEventArgs? e)
		{
			e = e ?? new SocketAsyncEventArgs();

			e.Completed += (object sender, SocketAsyncEventArgs e) =>
			{
				AcceptCompleted(sender, e);
			};

			if (ListenSocket.AcceptAsync(e) == false)
			{
				AcceptCompleted(this, e);
			}
		}

		private void AcceptCompleted(object sender, SocketAsyncEventArgs e)
		{
			if (e != null && ConnectedEvents.ContainsKey(e.GetHashCode()) == false)
			{
				if (e.AcceptSocket.Connected)
				{
					StartReceive(e);

					ConnectedEvents.Add(e.GetHashCode(), e);

					Console.WriteLine($"클라이언트 연결됨: {e.AcceptSocket.RemoteEndPoint}");
				}
			}

			// 연결 수락 후 다시 연결 대기
			StartAccept(new SocketAsyncEventArgs());
		}

		private void StartReceive(SocketAsyncEventArgs e)
		{
			e.SetBuffer(new byte[MaxPacektSize], 0, MaxPacektSize);

			e.Completed += (object sender, SocketAsyncEventArgs e) =>
			{
				if(e.BytesTransferred != 0)
					ReceiveCompleted(sender, e);
			};

			if (e.AcceptSocket.ReceiveAsync(e) == false)
			{
				ReceiveCompleted(this, e);
			}
		}
		PacketHeader DeserializePacketHeader(byte[] buffer, out int headerSize)
		{
			PacketHeader header = new PacketHeader();
			headerSize = Marshal.SizeOf<PacketHeader>();
			header.PacketType = BitConverter.ToUInt32(buffer.AsSpan(0, 4));
			header.PacketSize = BitConverter.ToUInt32(buffer.AsSpan(4, 4));
			return header;
		}
		private void ReceiveCompleted(object sender, SocketAsyncEventArgs e)
		{
			if (e.BytesTransferred > 0 && e.BytesTransferred <= e.Count)
			{
				int ReadCount = 0;
				while (ReadCount < e.BytesTransferred)
				{
					PacketHeader header = DeserializePacketHeader(e.Buffer, out int headerSize);
					if (((PacketType)header.PacketType == PacketType.PtNone) ||
						((PacketType)header.PacketType > PacketType.PtMax))
					{
						return;
					}

					ReadCount += headerSize;
					Span<byte> data = e.Buffer.AsSpan(ReadCount, (int)header.PacketSize);

					PacketHandler.HandleJob(e, header, data.ToArray());

					ReadCount += (int)header.PacketSize;
				}
				// 계속 데이터를 비동기적으로 받음
				StartReceive(e);
			}
			else
			{
				if (ConnectedEvents.ContainsKey(e.GetHashCode()))
				{					
					SocketAsyncEventArgs disconnected = ConnectedEvents[e.GetHashCode()];

					var foundClient = ConnectedClients.FirstOrDefault(elem =>elem.Value.GetHashCode() == disconnected.AcceptSocket.GetHashCode());
					if (foundClient.Value != null)
					{
						Console.WriteLine("클라이언트 연결 종료");
						ConnectedClients.Remove(foundClient.Key);
					}

					var foundDedi = ConnectedDedicateServer.FirstOrDefault(elem => elem.Value.Socket == disconnected.AcceptSocket);
					if (foundDedi.Value.Socket != null)
					{						
						Console.WriteLine($"{foundDedi.Value.IP}:{foundDedi.Value.Port} 데디케이트서버 연결 종료");
						ConnectedDedicateServer.Remove(foundDedi.Key);
					}
					e.AcceptSocket.Close();

				}
			}
		}

		public byte[] MakePacket<T>(PacketType packetType, T Data) 
			where T : Google.Protobuf.IMessage
		{
			PacketHeader header = new PacketHeader(packetType, (UInt32)Data.CalculateSize());
			byte[] typeBuffer = BitConverter.GetBytes(header.PacketType);
			byte[] sizeBuffer = BitConverter.GetBytes(header.PacketSize);
			byte[] serializedPacket = Data.ToByteArray();

			MemoryStream stream = new MemoryStream();
			stream.Write(typeBuffer);
			stream.Write(sizeBuffer);
			stream.Write(serializedPacket);

			byte[] headerIncludedPacket = stream.ToArray();
			return headerIncludedPacket;
		}

		public void SendPacket<T>(Socket clientSocket, PacketType packetType,T Data)
			where T : Google.Protobuf.IMessage
		{			
			clientSocket.SendAsync(MakePacket<T>(packetType, Data));
		}

		// 한번에 보내기 가능, 대신 패킷 조립해서 딱딱넣어놔야함
		public void SendRaw(Socket clientSocket,byte[] serializedPacket)
		{
			clientSocket.SendAsync(serializedPacket);
		}



		public void AcceptAsDedicateServer(ConnectedDedicateServerInfo dedicateServerInfo)
		{
			foreach (SocketAsyncEventArgs socketEvent in ConnectedEvents.Values)
			{
				if (socketEvent.AcceptSocket.GetHashCode() == dedicateServerInfo.Socket.GetHashCode())
				{
					ConnectedDedicateServer.Add(dedicateServerInfo.Port, dedicateServerInfo);
					break;
				}
			}
		}

		public void AcceptAsClient(Socket? socket, string id)
		{
			foreach (SocketAsyncEventArgs socketEvent in ConnectedEvents.Values)
			{
				if (socketEvent.AcceptSocket.GetHashCode() == socket.GetHashCode())
				{
					if (!ConnectedClients.ContainsKey(id))
					{
						ConnectedClients.Add(id, socketEvent.AcceptSocket);
					}
					break;
				}
			}
		}

		public void CloseServer()
		{
			ListenSocket.Close();
			foreach ( KeyValuePair<int, SocketAsyncEventArgs> Event in ConnectedEvents)
			{
				Console.WriteLine($"소켓 종료{Event.Value.AcceptSocket.RemoteEndPoint}");
				Event.Value.AcceptSocket.Disconnect(false);
				Event.Value.AcceptSocket.Close();
			}
			ConnectedEvents.Clear();
			PacketHandler.EndJob();

			CancelTokenSource.Cancel();
			if (ThreadManager.Instance != null)
			{
				ThreadManager.Instance.EndThreadManager();
			}
			if (DBCore.Instance != null)
			{
				DBCore.Instance.EndDB();
			}
			Console.WriteLine($"서버가 종료됨");
		}

		public void ReadCommand(string Command)
		{
			// 서버 명령어, 테스트용으로 사용가능
			Command = Command.ToLower();
			string[] CommandArgs = Command.Split(' ');
			switch (CommandArgs[0])
			{
				case "viewallclient":
					{
						Console.WriteLine("연결된 클라이언트 ID");
						foreach (string ID in ConnectedClients.Keys)
						{
							Console.WriteLine(ID);
						}
						break;
					}
				case "forcerundedi":
					{
						GameInstanceManager.Instance.RunAnyDedicateServer(out string batcherID,out string port);
						break;
					}					
				case "forceexitdedi":
					{
						if (CommandArgs.Length > 1)
						{
							GameInstanceManager.Instance.EndDedicateServer(CommandArgs[1]);
						}
						break;
					}
				case "exit":
					{
						CloseServer();
						break;
					}	
			}
		}

	}
}


