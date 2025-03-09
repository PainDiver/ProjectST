using MongoDB.Bson.Serialization.Serializers;
using Newtonsoft.Json.Linq;
using STNetServer.Core.ServerThread.Enums;
using STNetServer.Core.Utils;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace STNetServer.Core.Jobs
{
	public struct PacketHeader
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
		private ConcurrentQueue<JobRequest> JobQueue;
		

		public PacketHandler(int workerThreadCount)
		{
			Jobs = new Dictionary<PacketType, IJob>();
			JobQueue = new ConcurrentQueue<JobRequest>();
			
			RegisterJob(PacketType.PtCsLogin, new Job_Login());
			RegisterJob(PacketType.PtCsMatch, new job_Match());

			workerThreadCount = workerThreadCount < 1? 1 :workerThreadCount;
			int threadOffset = (int)ServerThread_NamedThread.JobWorkerStart + 1;
			for (int i=0;i<workerThreadCount;i++)
			{
				ThreadManager.Instance.DispatchThread(DoJob, threadOffset + i, 10);
			}
		}

		private void RegisterJob(PacketType type, IJob job)
		{
			Jobs.Add(type, job);
		}

		public void HandleJob(SocketAsyncEventArgs e,PacketHeader header, byte[] data)
		{
			JobQueue.Enqueue(new JobRequest(e.AcceptSocket, header, data));
		}

		public void EndJob()
		{
			foreach (KeyValuePair<PacketType,IJob> job in Jobs)
			{
				job.Value.Finish();
			}
			Jobs.Clear();
		}

		private void DoJob(object? state) //Worker가 쓸 함수임
		{
			ThreadInfo threadInfo = (ThreadInfo)state;
			try
			{
				while (!threadInfo.CancelToken_Global.IsCancellationRequested ||
					!threadInfo.CancelToken_Individual.IsCancellationRequested)
				{
					if (JobQueue.TryDequeue(out JobRequest job))
					{
						Console.WriteLine($"Worker Thread {threadInfo.ThreadID}: Packet : {job.Header.PacketType.ToString()} 처리 중");

						if (Jobs.ContainsKey((PacketType)job.Header.PacketType))
						{
							Jobs[(PacketType)job.Header.PacketType].Execute(job.ClientSocket, job.Header, job.Data);
						}
					}
					else
					{
						int breakTime = (int)threadInfo.AdditionalData;
						Thread.Sleep(breakTime);
					}
				}
			}
			catch (Exception e) 
			{
				Console.WriteLine($"Worker Thread 오류 :{e.Message}");
			}
		}
	}
}
