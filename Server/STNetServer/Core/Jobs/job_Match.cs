using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal class job_Match : IJob
	{
		public async void Execute(SocketAsyncEventArgs e, PacketHeader type, byte[] data)
		{
			Console.WriteLine("Match Job Done!");
		}
	}
}
