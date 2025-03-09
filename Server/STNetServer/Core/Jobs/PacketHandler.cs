using MongoDB.Bson.Serialization.Serializers;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace STNetServer.Core.Jobs
{
	struct PacketHeader
	{
		public PacketHeader(PacketType type, UInt32 size)
		{
			PacketType = (UInt32)type;
			PacketSize = size;
		}

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

			RegisterJob(PacketType.PtCsLogin, new Job_Login());
			RegisterJob(PacketType.PtCsMatch, new job_Match());
		}

		public void HandleJob(SocketAsyncEventArgs e,PacketHeader header, byte[] data)
		{
			if (Jobs.ContainsKey((PacketType)header.PacketType))
			{
				Jobs[(PacketType)header.PacketType].Execute(e, header, data);
			}
		}
		private void RegisterJob(PacketType type, IJob job)
		{
			Jobs.Add(type, job);
		}

	}
}
