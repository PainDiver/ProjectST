using Google.Protobuf;
using STNetServer.Core.NewFolder;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal class Job_Dedi : IJob
	{
		public void Execute(Socket clientSocket, PacketHeader type, byte[] data)
		{
			DS_Packet_Dedi Packet = new DS_Packet_Dedi();
			Packet.MergeFrom(data);
			GameInstanceManager.Instance.MarkDedicateServerAlive(Packet.BatcherID,Packet.Port);

			lock (ServerCore.Instance)
			{
				ServerCore.Instance.AcceptAsDedicateServer(clientSocket,Packet.Port);
			}
			GameInstanceManager.Instance.MarkDedicateServerAlive(Packet.BatcherID, Packet.Port);


		}

		public void Finish()
		{
		}
	}
}
