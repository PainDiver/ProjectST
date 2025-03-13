using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Json
{
	public class ServerConfig
	{
		public string? ServerDNS { get; set; }
		public string? ServerPort { get; set; }
		public string? MongoDBIp { get; set; }
		public string? MongoDBPort { get; set; }
		public string? RedisDBIp { get; set; }
		public string? RedisDBPort { get; set; }
	}

	public class MailConfig
	{
		public string? AlerterEmail { get; set; }
		public string? AlerterPassword { get; set; }
		public string? ReceiverMail { get; set; }
		
	}
}
