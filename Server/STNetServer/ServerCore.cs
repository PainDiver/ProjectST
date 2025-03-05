using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;


namespace STNetServer
{
	class ServerCore
	{
		public int Port = 17777;
		public int MaxConnections = 1000;
		private Socket ListenSocket;
		
		public ServerCore(int NewPort)
		{
			Port = NewPort;
			ListenSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
		}

		public void StartServer()
		{
			IPEndPoint localEndPoint = new IPEndPoint(IPAddress.Any, Port);
			ListenSocket.Bind(localEndPoint);
			ListenSocket.Listen(MaxConnections);

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
			Console.WriteLine($"클라이언트 연결됨: {e.AcceptSocket.RemoteEndPoint}");
			StartReceive(e);

			// 연결 수락 후 다시 연결 대기
			StartAccept(new SocketAsyncEventArgs());
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
				Console.WriteLine("클라이언트 연결 종료");
				e.AcceptSocket.Shutdown(SocketShutdown.Both);
				e.AcceptSocket.Close();
			}
		}

		struct PacketHeader
		{
			public Span<byte> ReadBuffer(ref Span<byte> PacketBuffer)
			{
				Type = MemoryMarshal.Read<UInt32>(PacketBuffer.Slice(0, Marshal.SizeOf(Type)));
				Size = MemoryMarshal.Read<UInt32>(PacketBuffer.Slice(0, Marshal.SizeOf(Size)));

				return PacketBuffer.Slice(0,(int)Size);
			}

			public UInt32 Type;
			public UInt32 Size;
		}

		//이거 자동화필요
		enum EPacketType : UInt32
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
		

	}
}


