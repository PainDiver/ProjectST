using MongoDB.Driver;
using StackExchange.Redis;
using STNetServer.Global;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.DB
{
	
	internal class MongoDB : IDB
	{
		Process? MongoDBProcess;
		MongoClient Connection;
		public MongoDB(Process NewProcess) 
		{
			MongoDBProcess = NewProcess;
		}

		public void Connect()
		{
			Connection = new MongoClient($"mongodb://{GlobalConfig.Config.MongoDBIp}:{GlobalConfig.Config.MongoDBPort}");
		}


		public void Exit()
		{
			if (MongoDBProcess != null)
				MongoDBProcess.Kill();
		}
	}
}
