using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	public class JobRequest
	{
		public JobRequest(Socket clientSocket, PacketHeader header, byte[] data)
		{
			ClientSocket = clientSocket;
			Header = header;
			Data = data; 
		}

		public Socket ClientSocket { get; private set; }
		public PacketHeader Header { get; private set; }
		public byte[] Data { get; private set; }
	}

	internal interface IJob
	{
		public void Execute(Socket clientSocket, PacketHeader type, byte[] data);
		public void Finish();
	}
}
