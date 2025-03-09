using StackExchange.Redis;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.DB
{
	internal class Redis : IDB
	{
		Process? RedisProcess;
		ConnectionMultiplexer Connection;
		public IDatabase DataBase;

		public Redis(Process NewProcess) 
		{
			RedisProcess = NewProcess;
		}

		public void Connect()
		{
			Connection = ConnectionMultiplexer.Connect($"{GlobalVariables.Config.RedisDBIp}:{GlobalVariables.Config.RedisDBPort}");
			DataBase = Connection.GetDatabase();
		}

		public async Task WriteHashSet(string key, Dictionary<string,string> values)
		{
			List<HashEntry> hashEntries = new List<HashEntry>();
			foreach ( var value in values)
			{
				hashEntries.Add(new HashEntry(value.Key, value.Value));
			}
			await DataBase.HashSetAsync(key, hashEntries.ToArray());
		}

		public async Task<HashEntry[]> ReadHashSet(string key)
		{
			return await DataBase.HashGetAllAsync(key);
		}

		public async Task WriteSet(string key, string value)
		{
			await DataBase.SetAddAsync(key, value);
		}
		public async Task<RedisValue[]> ReadSet(string key)
		{
			return await DataBase.SetMembersAsync(key);
		}

		public async Task WriteValue(string key, string value)
		{
			await DataBase.StringSetAsync(key, value);
		}

		public async Task<RedisValue> ReadValue(string key)
		{
			return await DataBase.StringGetAsync(key);
		}

		public void Exit()
		{
			if(RedisProcess != null)
				RedisProcess.Kill();
		}
	}
}
