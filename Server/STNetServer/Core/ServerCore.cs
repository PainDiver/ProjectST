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
using STNetUtils.RESTContext;


namespace STNetServer.Core
{
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
		Dictionary<string, Socket> ConnectedDedicateServer;
		
		public Socket GetDedicateServer(string port)
		{
			Socket socket;
			ConnectedDedicateServer.TryGetValue(port,out socket);
			return socket;
		}


		PacketHandler? PacketHandler;

		public int MaxConnections = 100;
		public int MaxPacektSize = 1024;

		private ServerCore()
		{
			ListenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			ConnectedClients = new Dictionary<string,Socket>();
			ConnectedDedicateServer = new Dictionary<string,Socket>();
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
						if (ConnectedEvents.ContainsKey(e.GetHashCode()))
						{
							ConnectedEvents.Remove(e.GetHashCode());
							e.AcceptSocket.Close();
							Console.WriteLine("비정상 클라이언트 연결 종료");
						}
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
					ConnectedEvents.Remove(e.GetHashCode());
					e.AcceptSocket.Close();
					Console.WriteLine("클라이언트 연결 종료");
				}
			}
		}


		public void Send(Socket clientSocket, PacketType packetType, byte[] serializedPacket, int size)
		{
			if (clientSocket == null)
			{
				return;
			}
			PacketHeader header = new PacketHeader(packetType, (UInt32)size);
			byte[] typeBuffer = BitConverter.GetBytes(header.PacketType);
			byte[] sizeBuffer = BitConverter.GetBytes(header.PacketSize);

			MemoryStream stream = new MemoryStream();
			stream.Write(typeBuffer);
			stream.Write(sizeBuffer);
			stream.Write(serializedPacket);

			clientSocket.SendAsync(stream.ToArray());
			stream.Close();
		}

		public void AcceptAsDedicateServer(Socket? socket,string port)
		{
			foreach (SocketAsyncEventArgs socketEvent in ConnectedEvents.Values)
			{
				if (socketEvent.AcceptSocket.GetHashCode() == socket.GetHashCode())
				{
					ConnectedDedicateServer.Add(port, socketEvent.AcceptSocket);
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
					ConnectedClients.Add(id, socketEvent.AcceptSocket);
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
				case "forcerundedi":
					{
						GameInstanceManager.Instance.RunAnyDedicateServer();
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


