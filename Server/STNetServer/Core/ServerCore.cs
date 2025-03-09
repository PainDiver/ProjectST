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


namespace STNetServer.Core
{
	class ServerCore
	{
		private static readonly Lazy<ServerCore> ServerCoreInstance = new Lazy<ServerCore>(() => new ServerCore());

		public static ServerCore Instance => ServerCoreInstance.Value;


		public int Port = 17777;
		public int MaxConnections = 1000;
		private Socket ListenSocket;
		private bool bIsServerActive;
		Dictionary<int, SocketAsyncEventArgs> ConnectedEvents;
		PacketHandler PacketHandler;

		public bool IsServerActive
		{
			get
			{
				return bIsServerActive;
			}
		}			
		private ServerCore()
		{
			ListenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			ConnectedEvents = new Dictionary<int, SocketAsyncEventArgs>();
			PacketHandler = new PacketHandler();
		}
		~ServerCore()
		{
			EndSocket();
		}

		public void StartServer()
		{
			IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, Port);
			ListenSocket.Bind(localEndPoint);
			ListenSocket.Listen(MaxConnections);
			bIsServerActive = true;
			Console.WriteLine($"서버가 {Port} 포트에서 시작 됨");
			// 클라이언트 연결을 기다리며 비동기 작업 시작
			StartAccept(null);
		}

		private void StartAccept(SocketAsyncEventArgs e)
		{
			e = e ?? new SocketAsyncEventArgs();
			e.Completed += AcceptCompleted;
			if (ListenSocket.AcceptAsync(e) == false)
			{
				AcceptCompleted(this, e);
			}
		}

		private void AcceptCompleted(object sender, SocketAsyncEventArgs e)
		{
			if (e != null && ConnectedEvents.ContainsKey(e.GetHashCode()) == false)
			{
				StartReceive(e);

				// Hello Test
				if (e.AcceptSocket.Connected)
				{
					ConnectedEvents.Add(e.GetHashCode(), e);

					Console.WriteLine($"클라이언트 연결됨: {e.AcceptSocket.RemoteEndPoint}");
					// 연결 수락 후 다시 연결 대기
					StartAccept(new SocketAsyncEventArgs());
				}
			}
		}

		private void StartReceive(SocketAsyncEventArgs e)
		{
			e.SetBuffer(new byte[1024], 0, 1024);
			e.Completed += ReceiveCompleted;
			if (e.AcceptSocket.ReceiveAsync(e) == false)
			{
				ReceiveCompleted(this, e);
			}
		}

		private void ReceiveCompleted(object sender, SocketAsyncEventArgs e)
		{
			if (e.BytesTransferred > 0 && e.BytesTransferred <= e.Count)
			{
				int ReadCount = 0;
				while (ReadCount < e.BytesTransferred)
				{
					PacketHeader header = new PacketHeader();
					int headerSize = Marshal.SizeOf<PacketHeader>();
					header.PacketType = BitConverter.ToUInt32(e.Buffer.AsSpan(ReadCount, 4));
					ReadCount += 4;
					header.PacketSize = BitConverter.ToUInt32(e.Buffer.AsSpan(ReadCount, 4));
					ReadCount += 4;

					Span<byte> data = e.Buffer.AsSpan(ReadCount,(int)header.PacketSize);					
					PacketHandler.HandleJob(e,header, data.ToArray());
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


		public void Send(SocketAsyncEventArgs e,PacketType packetType, byte[] serializedPacket,int size)
		{
			PacketHeader header = new PacketHeader(packetType, (UInt32)size);
			byte[] typeBuffer = BitConverter.GetBytes(header.PacketType);
			byte[] sizeBuffer = BitConverter.GetBytes(header.PacketSize);

			MemoryStream stream = new MemoryStream();
			stream.Write(typeBuffer);
			stream.Write(sizeBuffer);
			stream.Write(serializedPacket);
			
			e.AcceptSocket.SendAsync(stream.ToArray());
			stream.Close();
		}

		public void EndSocket()
		{
			ListenSocket.Close();
			bIsServerActive = false;
			foreach ( KeyValuePair<int, SocketAsyncEventArgs> Event in ConnectedEvents)
			{
				Console.WriteLine($"소켓 종료{Event.Value.AcceptSocket.RemoteEndPoint}");
				Event.Value.AcceptSocket.Disconnect(false);
				Event.Value.AcceptSocket.Close();
			}
			ConnectedEvents.Clear();
			Console.WriteLine($"서버가 종료됨");
		}

		public void ReadCommand(string Command)
		{
			Command = Command.ToLower();
			switch (Command)
			{
				case "exit":
					{
						EndSocket();
					}
					break;			
			}
		}

	}
}


