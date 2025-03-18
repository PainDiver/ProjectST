using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;
using Google.Protobuf;
using StackExchange.Redis;
using STNetServer.Core.DB;
using STNetServer.Core.DB.MongoDB;

namespace STNetServer.Core.Jobs
{
	internal class Job_Login : IJob
	{
		public async void Execute(Socket clientSocket, PacketHeader type, byte[] data)
		{
			CS_Packet_Login Packet = new CS_Packet_Login();
			Packet.MergeFrom(data);
			
			// Redis에 
			Dictionary<string,string> hashEntires = new Dictionary<string,string>();
			hashEntires.Add("LoginTime", DateTimeOffset.UtcNow.ToString());
			await DBCore.Instance.RedisInstance.WriteHashSet(Packet.UserId, hashEntires);

			DB_UserAccount account = await DBCore.Instance.MongoDBInstance.Read<DB_UserAccount, string>(
				"GameDB", "UserAccount", DB_UserAccount.GetKeyPropertyName(), Packet.UserId);

			SC_Packet_Login SendPacket = new SC_Packet_Login();
			//SendPacket.AccountData = new AccountData();
			if (account == null)
			{
				account = new DB_UserAccount(Packet.UserId, Packet.Password);
				await DBCore.Instance.MongoDBInstance.Write<DB_UserAccount>("GameDB", "UserAccount", account);
				Console.WriteLine("계정 정보 찾지 못함 새로운 계정 추가");
			}
			else
			{
				SendPacket.IsAccountCreated = true;
				Console.WriteLine("계정 정보 찾음 Login 가능함");
			}
			//SendPacket.AccountData.ID = account.ID;

			lock (ServerCore.Instance)
			{
				ServerCore.Instance.AcceptAsClient(clientSocket,Packet.UserId);
			}

			Console.WriteLine($"클라이언트 ID:{Packet.UserId},Password:{Packet.Password} 전달받음! 로그인 성공패킷을 보냄!");

			ServerCore.Instance.SendPacket<SC_Packet_Login>(clientSocket, PacketType.PtScLogin, SendPacket);
			Console.WriteLine("Login Job Done!");
		}

		public void Finish()
		{
		}
	}
}
