using StackExchange.Redis;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using STNetServer.Json;
using System.ComponentModel;

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
		public MongoDB MongoDBInstance => (MongoDB)DBprocesses[DBType.MONGODB];

		private DBCore()
		{
			DBDirectory = Directory.GetCurrentDirectory() + "\\..\\..\\..\\..\\DB";
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

			string MongoDBIP = GlobalVariables.Config.MongoDBIp;
			string MongoDBPort = GlobalVariables.Config.MongoDBPort;
			Process MongoDBProcess = null;
			try
			{
				MongoDBProcess = RunProgram(MongoDBServer + "\\bin\\mongod", $"--dbpath {MongoDBDataPath} --port {MongoDBPort} --bind_ip {MongoDBIP}");
			}
			catch (Exception ex)
			{
				Console.WriteLine("MongoDB Failed to run");
				EndDB();
			}
			MongoDB MongoDBInstance = new MongoDB(MongoDBProcess);
			DBprocesses.Add(DBType.MONGODB, MongoDBInstance);


			string RedisIP = GlobalVariables.Config.RedisDBIp;
			string RedisPort = GlobalVariables.Config.RedisDBPort;
			Process RedisProcess = null;
			try
			{
				RedisProcess = RunProgram(DBDirectory + "\\Redis\\redis-server", $"--port {RedisPort} --bind {RedisIP}");
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

		static Process RunProgram(string Path, string Args)
		{
			Process process = new Process();
			Console.WriteLine($"Running Program : {Path}");
			process.StartInfo.FileName = Path;  // protoc 실행 파일
			process.StartInfo.Arguments = Args;
			process.StartInfo.UseShellExecute = true;
			process.StartInfo.Verb = "runas";
			process.Start();

			return process;
		}
	}
}
