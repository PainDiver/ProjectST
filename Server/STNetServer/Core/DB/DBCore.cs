using StackExchange.Redis;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using STNetServer.Json;
using System.ComponentModel;
using STNetServer.Global;
using STNetServer.Core.DB.MongoDB;

namespace STNetServer.Core.DB
{
	enum DBType
	{
		MONGODB,
		REDIS
	}
	public interface IDB
	{
		public void Exit();
		public void Connect();
	}


	class DBCore
	{
		private static Lazy<DBCore> DBCoreInstance = new Lazy<DBCore>(() => new DBCore());
		public static DBCore Instance => DBCoreInstance.Value;

		private string DBDirectory;
		private Dictionary<DBType, IDB> DBprocesses;

		public Redis RedisInstance => (Redis)DBprocesses[DBType.REDIS];
		public MongoDBCore MongoDBInstance => (MongoDBCore)DBprocesses[DBType.MONGODB];

		private DBCore()
		{
#if DEBUG
			DBDirectory = Directory.GetCurrentDirectory() + "\\..\\DB";
#else
			DBDirectory = Directory.GetCurrentDirectory() + "\\DB";
#endif
			DBprocesses = new Dictionary<DBType, IDB>();
		}

		~DBCore()
		{
			EndDB();
		}

		
		public bool StartDBServer()
		{

			string MongoDBServer = DBDirectory + "\\MongoDB";
			string MongoDBDataPath = MongoDBServer + "\\data\\db";

			string? MongoDBIP = GlobalConfig.Config.MongoDBIp;
			string? MongoDBPort = GlobalConfig.Config.MongoDBPort;
			Process? MongoDBProcess = null;
			try
			{
				MongoDBProcess = GlobalFunction.RunProgram(MongoDBServer + "\\bin\\mongod", $"--dbpath {MongoDBDataPath} --port {MongoDBPort} --bind_ip {MongoDBIP}");
			}
			catch
			{
				Console.WriteLine("MongoDB Failed to run");
				EndDB();
			}
			MongoDBCore MongoDBInstance = new MongoDBCore(MongoDBProcess);
			DBprocesses.Add(DBType.MONGODB, MongoDBInstance);


			string? RedisIP = GlobalConfig.Config.RedisDBIp;
			string? RedisPort = GlobalConfig.Config.RedisDBPort;
			Process RedisProcess = null;
			try
			{
				RedisProcess = GlobalFunction.RunProgram(DBDirectory + "\\Redis\\redis-server", $"--port {RedisPort} --bind {RedisIP}");
			}
			catch
			{
				Console.WriteLine("Redis Failed to run");
				EndDB();
			}
			Redis RedisInstance = new Redis(RedisProcess);
			DBprocesses.Add(DBType.REDIS, RedisInstance);

			Console.WriteLine("MongoDB Server Starts Running!");
			Console.WriteLine("Redis Server Starts Running");

			foreach (KeyValuePair<DBType,IDB> db in DBprocesses)
			{
				db.Value.Connect();
			}
			return true;
		}


		public void EndDB()
		{
			foreach (KeyValuePair<DBType, IDB> Process in DBprocesses)
			{
				Process.Value.Exit();
			}
			DBprocesses.Clear();
		}
	}
}
