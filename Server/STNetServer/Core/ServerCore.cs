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


namespace STNetServer.Core
{
	class ServerCore
	{
		public int Port = 17777;
		public int MaxConnections = 1000;
		private Socket ListenSocket;
		private bool bIsServerActive;
		Dictionary<int, SocketAsyncEventArgs> ConnectedEvents;

		public bool IsSocketActive
		{
			get
			{
				
				return bIsServerActive;
			}
		}			
		public ServerCore(int NewPort)
		{
			Port = NewPort;
			ListenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
			ConnectedEvents = new Dictionary<int, SocketAsyncEventArgs>();
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
			StartReceive(e);

			// Hello Test
			if (e.AcceptSocket.Connected)
			{
				ConnectedEvents.Add(e.GetHashCode(), e);
				
				//Console.WriteLine($"클라이언트 연결됨: {e.AcceptSocket.RemoteEndPoint}");
				//UInt32 PacketType = 1;
				//UInt32 PacketSize = 0;
				//MemoryStream ms = new MemoryStream();
				//ms.Write(BitConverter.GetBytes(PacketType), 0, sizeof(UInt32));
				//ms.Write(BitConverter.GetBytes(PacketSize), 0, sizeof(UInt32));
				//e.AcceptSocket.Send(ms.ToArray());
			
				// 연결 수락 후 다시 연결 대기
				StartAccept(new SocketAsyncEventArgs());
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
				PacketHeader header = new PacketHeader();
				Span<byte> buffer = e.Buffer.AsSpan();
				//버퍼 소진될때까지
				while (buffer.Length > 0)
				{
					Span<byte> data = header.ReadBuffer(ref buffer);
					DoJob(header, data);
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

		void EndSocket()
		{
			ListenSocket.Close();
			bIsServerActive = false;
		}

		struct PacketHeader
		{
			public Span<byte> ReadBuffer(ref Span<byte> PacketBuffer)
			{
				Type = MemoryMarshal.Read<uint>(PacketBuffer.Slice(0, Marshal.SizeOf(Type)));
				Size = MemoryMarshal.Read<uint>(PacketBuffer.Slice(0, Marshal.SizeOf(Size)));

				return PacketBuffer.Slice(0, (int)Size);
			}

			public uint Type;
			public uint Size;
		}

		//이거 자동화필요
		enum EPacketType : uint
		{
			PT_LOGIN = 1,
			PT_MATCH = 2
		}

		private void DoJob(PacketHeader Header, Span<byte> Data)
		{
			switch ((EPacketType)Header.Type)
			{
				case EPacketType.PT_LOGIN:
					{

					}
					break;
				case EPacketType.PT_MATCH:
					{

					}
					break;
				default:
					break;
			}
		}


		public void ReadCommand(string Command)
		{
			switch (Command)
			{
				case "Exit":
					{
						EndSocket();
					}
					break;			
			}
		}

	}
}


