using StackExchange.Redis;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core
{
	enum DBType
	{
		MONGODB,
		REDIS
	}

	class DBCore
	{
		private string DBDirectory;
		private Dictionary<DBType, Process> DBprocesses;

		public DBCore(string NewDBDirectory)
		{
			DBDirectory = NewDBDirectory;
			DBprocesses = new Dictionary<DBType, Process>();
		}

		public bool StartDB()
		{
			string MongoDBServer = DBDirectory + "\\MongoDB";
			string MongoDBDataPath = MongoDBServer + "\\data\\db";
			string MongoDBIP = "127.0.0.1";
			string MongoDBPort = "27017";
			Process MongoDB = null;
			try
			{
				MongoDB = RunProgram(MongoDBServer + "\\bin\\mongod", $"--dbpath {MongoDBDataPath} --port {MongoDBPort} --bind_ip {MongoDBIP}");
			}
			catch (Exception ex)
			{
				Console.WriteLine("MongoDB Failed to run");
				EndDB();
			}
			DBprocesses.Add(DBType.MONGODB, MongoDB);

			string RedisIP = "127.0.0.1";
			string RedisPort = "6379";
			Process Redis = null;
			try
			{
				Redis = RunProgram(DBDirectory + "\\Redis\\redis-server", $"--port {RedisPort} --bind {RedisIP}");
			}
			catch
			{
				Console.WriteLine("Redis Failed to run");
				EndDB();
			}
			DBprocesses.Add(DBType.REDIS, Redis);

			Console.WriteLine("MongoDB Starts Running!");
			Console.WriteLine("Redis Starts Running");
			return true;
		}

		void EndDB()
		{ 
			foreach (KeyValuePair<DBType,Process> Process in DBprocesses)
			{
				Process.Value.Kill();
			}
			DBprocesses.Clear();
		}

		static Process RunProgram(string Path, string Args)
		{
			Process process = new Process();
			process.StartInfo.FileName = Path;  // protoc 실행 파일
			process.StartInfo.Arguments = Args;
			process.StartInfo.UseShellExecute = false;
			process.StartInfo.RedirectStandardOutput = true;
			process.Start();

			return process;
		}
	}
}
