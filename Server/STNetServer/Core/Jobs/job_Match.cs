using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal class job_Match : IJob
	{
		public void Execute(ServerCore Core, PacketHeader type, Span<byte> data)
		{
			Console.WriteLine("Match Job Done!");
		}
	}
}
