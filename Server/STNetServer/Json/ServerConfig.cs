using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Json
{
	public class ServerConfig
	{
		public string ServerPort { get; set; }
		public string MongoDBIp { get; set; }
		public string MongoDBPort { get; set; }
		public string RedisDBIp { get; set; }
		public string RedisDBPort { get; set; }
	}
}
