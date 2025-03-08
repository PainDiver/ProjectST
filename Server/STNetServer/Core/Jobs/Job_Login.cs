using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Cryptography.X509Certificates;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal class Job_Login : IJob
	{
		public void Execute(ServerCore Core,PacketHeader type, Span<byte> data)
		{
			Console.WriteLine("Login Job Done!");
		}
	}
}
