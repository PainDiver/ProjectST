using MongoDB.Driver;
using StackExchange.Redis;
using STNetServer.Global;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Linq.Expressions;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Core.DB.MongoDB
{

	internal class MongoDBCore : IDB
	{
		Process? MongoDBProcess;
		MongoClient? Connection;
		public MongoDBCore(Process NewProcess)
		{
			MongoDBProcess = NewProcess;
		}

		public void Connect()
		{
			Connection = new MongoClient($"mongodb://{GlobalConfig.Config.MongoDBIp}:{GlobalConfig.Config.MongoDBPort}");


		}

		public async Task Write<T>(string dataBaseName, string collectionName, T data)
		{
			IMongoDatabase db = Connection.GetDatabase(dataBaseName);
			IMongoCollection<T> collection = db.GetCollection<T>(collectionName);

			await collection.InsertOneAsync(data);
		}

		public async Task<DataType> Read<DataType, KeyType>(string dbName, string collectionName, string keyPropertyName /*멤버이름*/, KeyType keyValue) 
			where DataType : class
		{
			// 데이터베이스와 컬렉션 선택
			IMongoDatabase database = Connection.GetDatabase(dbName);
			IMongoCollection<DataType> collection = database.GetCollection<DataType>(collectionName);

			// 동적으로 키 값을 기준으로 쿼리
			var filter = Builders<DataType>.Filter.Eq(keyPropertyName, keyValue);

			return await collection.Find(filter).FirstOrDefaultAsync();
		}
		//private FilterDefinition<T> BuildFilter<T, TKey>(string keyName, TKey keyValue)
		//{
		//	// 필터 조건: keyName == keyValue
		//	var filterBuilder = Builders<T>.Filter;
		//	var keyProperty = Expression.Property(Expression.Parameter(typeof(T), "x"), keyName);
		//	var constant = Expression.Constant(keyValue);

		//	var expression = Expression.Equal(keyProperty, constant);
		//	var lambda = Expression.Lambda<Func<T, bool>>(expression, Expression.Parameter(typeof(T), "x"));

		//	var filter = filterBuilder.Where(lambda);
		//	return filter;
		//}


		public void Exit()
		{
			if (MongoDBProcess != null)
				MongoDBProcess.Kill();
		}
	}
}
