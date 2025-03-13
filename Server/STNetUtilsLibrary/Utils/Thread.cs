using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace STNetUtils.Thread
{
	public enum ServerThread_NamedThread
	{ 
		AccRecvWorkerStart = 0,

		AccRecvWorker1 = 1,
		AccRecvWorker2 = 2,
		AccRecvWorker3 = 3,
		AccRecvWorker4 = 4,
		AccRecvWorker5 = 5,
		AccRecvWorker6 = 6,
		AccRecvWorker7 = 7,
		AccRecvWorker8 = 8,
		AccRecvWorker9 = 9,
		AccRecvWorker10 = 10,

		JobWorkerStart = 11,

		JobWorker1 = 12,
		JobWorker2 = 13,
		JobWorker3 = 14,
		JobWorker4 = 15,
		JobWorker5 = 16,
		JobWorker6 = 17,
		JobWorker7 = 18,
		JobWorker8 = 19,
		JobWorker9 = 20,
		JobWorker10 = 21,
	}
}

namespace STNetServer.Global
{	public static class GlobalThreadFunc
	{
		public static T InterpretIDAs<T>(int ID)
		{
			T Type = (T)Enum.ToObject(typeof(T), ID);
			return Type;
		}
	}
	
}

namespace STNetServer.Core.Utils
{

	public class ThreadManager
	{
		private static readonly Lazy<ThreadManager> Manager = new Lazy<ThreadManager>(() => new ThreadManager());
		public static ThreadManager Instance => Manager.Value;

		private Dictionary<int, ThreadController> ManagedThreads;
		private CancellationTokenSource GlobalTokenSource;

		private ThreadManager() 
		{
			GlobalTokenSource = new CancellationTokenSource();
			ManagedThreads = new Dictionary<int, ThreadController>();
		}

		public void DispatchThread(WaitCallback CallBack,int threadID,object AddtionalData)
		{
			if (ManagedThreads.ContainsKey(threadID))
			{
				Console.WriteLine("Same Thread ID already Exists");
				
				return;
			}

			CancellationTokenSource cancelTokenSource = new CancellationTokenSource();
			ThreadInfo threadInfo = new ThreadInfo(threadID, GlobalTokenSource.Token, cancelTokenSource.Token, AddtionalData);

			CallBack += OnThreadFinished;

			ManagedThreads.Add(threadInfo.ThreadID, new ThreadController(threadInfo, cancelTokenSource));
			ThreadPool.QueueUserWorkItem(CallBack, threadInfo);

		}

		public void EndThread(int ThreadID)
		{
			if (ManagedThreads.ContainsKey(ThreadID))
			{
				ManagedThreads[ThreadID].CancelTokenSource.Cancel();
			}
		}

		public void EndThreadManager()
		{
			GlobalTokenSource.Cancel();
			foreach (KeyValuePair<int,ThreadController> thread in ManagedThreads)
			{
				thread.Value.CancelTokenSource.Cancel();
			}
			ManagedThreads.Clear();
		}

		void OnThreadFinished(object? state)
		{
#if DEBUG
			ThreadInfo threadInfo = (ThreadInfo)state;
			Console.WriteLine($"Thread {threadInfo.ThreadID} Finished Working");
#endif
		}

	}


	class ThreadController
	{
		public ThreadController(ThreadInfo threadInfo, CancellationTokenSource cancelTokenSource)
		{
			ThreadInfo = threadInfo;
			CancelTokenSource = cancelTokenSource;
		}

		public ThreadInfo ThreadInfo;
		public CancellationTokenSource CancelTokenSource;
	}
	public struct ThreadInfo
	{
		public ThreadInfo(int threadID ,CancellationToken serverToken, CancellationToken individualToken,object? additionalData)
		{
			ThreadID = threadID;
			CancelToken_Global = serverToken;
			CancelToken_Individual = individualToken;
			AdditionalData = additionalData;

		}


		public int ThreadID;
		public CancellationToken CancelToken_Global;
		public CancellationToken CancelToken_Individual;
		public object? AdditionalData;
	}
}
