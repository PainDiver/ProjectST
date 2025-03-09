using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal interface IJob
	{
		public void Execute(SocketAsyncEventArgs e,PacketHeader type, byte[] data);
		
	}
}
