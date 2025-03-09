using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using STNetServer.Core.DB;

namespace STNetServer.Core.Jobs
{
	internal class Job_Login : IJob
	{
		public async void Execute(SocketAsyncEventArgs e,PacketHeader type, byte[] data)
		{
			CS_Packet_Login Packet = new CS_Packet_Login();
			Packet.MergeFrom(data);
			
			Dictionary<string,string> hashEntires = new Dictionary<string,string>();
			hashEntires.Add("Password", Packet.Password);
			hashEntires.Add("LoginTime", DateTimeOffset.UtcNow.ToString());

			await DBCore.Instance.RedisInstance.WriteHashSet(Packet.UserId, hashEntires);

			SC_Packet_Login SendPacket = new SC_Packet_Login();
			SendPacket.Result = true;

			ServerCore.Instance.Send(e,PacketType.PtScLogin, SendPacket.ToByteArray(), SendPacket.CalculateSize());
			Console.WriteLine("Login Job Done!");
		}
	}
}
