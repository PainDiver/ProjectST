using STNetServer.Core.Jobs;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace STNetServer.Core
{
	struct PacketHeader
	{
		public UInt32 PacketType;
		public UInt32 PacketSize;
	};

	internal class PacketHandler
	{
		public delegate void JobDelegate(PacketHeader header, Span<byte> data);		
		private Dictionary<PacketType, IJob> Jobs;

		public PacketHandler()
		{
			Jobs = new Dictionary<PacketType, IJob>();

			RegisterJob(PacketType.PtCsLogin,new Job_Login());
			RegisterJob(PacketType.PtCsMatch, new job_Match());
		}

		public void HandleJob(ServerCore Core,PacketHeader header, Span<byte> data)
		{
			if (Jobs.ContainsKey((PacketType)header.PacketType))
			{
				Jobs[(PacketType)header.PacketType].Execute(Core,header, data);
			}
		}
		private void RegisterJob(PacketType type, IJob job)
		{
			Jobs.Add(type,job);
		}

	}
}
