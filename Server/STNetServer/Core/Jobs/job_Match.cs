using Google.Protobuf;
using STNetServer.Core.DB;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using STNetUtils.REST;
using STNetUtils.RESTContext;
using STNetServer.Core.NewFolder;

namespace STNetServer.Core.Jobs
{
	internal class job_Match : IJob
	{
		public async void Execute(Socket clientSocket, PacketHeader type, byte[] data)
		{
			CS_Packet_Match Packet = new CS_Packet_Match();
			Packet.MergeFrom(data);

			GameInstanceManager.Instance.EnqueueClientToMatchingServer(new MatchingClientInfo(Packet.UserId));

			//MatchingPayload payload = await RESTHelper.SendHttpByJsonAsync<MatchingPayload>(
			//	GameInstanceServerContext.MatchingURL,
			//	GameInstanceServerContext.EnqueueClient,
			//	Packet.UserId);

			//if (payload != null)
			//{
			//	//Client 에게 전송

			//	SC_Packet_Match matchingPacket = new SC_Packet_Match();
			//	matchingPacket.DedicateServerIP = ""+ $"{payload.BoundDedicateServer.ServerPort}";
			//	byte[] serialized = matchingPacket.ToByteArray();
			//	ServerCore.Instance.Send(clientSocket,PacketType.PtScMatch, serialized, serialized.Length);
				
			//}
		}

		public void Finish()
		{
		}
	}
}
