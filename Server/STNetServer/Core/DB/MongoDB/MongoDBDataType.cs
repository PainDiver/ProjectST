using MongoDB.Bson.Serialization.Attributes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.DB.MongoDB
{

	public class DB_UserAccount
	{
		public DB_UserAccount(string id, string password)
		{
			ID = id;
			Password = password;
		}

		public static string GetKeyPropertyName()
		{
			return "ID";
		}

		[BsonId]
		public string ID;
		public string Password;
	}
}
