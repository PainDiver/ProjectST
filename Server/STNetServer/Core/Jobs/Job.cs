using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.Jobs
{
	internal interface IJob
	{
		void Execute(ServerCore Core, PacketHeader type, Span<byte> data);
		
	}
}
