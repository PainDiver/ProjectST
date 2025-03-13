using MongoDB.Bson.IO;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;
using STNetUtils.REST;
using System.Diagnostics;

namespace STNetUtils.REST
{
	public enum HTTPVerb
	{ 
		GET,
		POST,
		DELETE,
		PUT,
		PATCH
	}


	public class RESTHelper
	{
		
		private static readonly HttpClient client = new HttpClient();

		public static async Task<T> SendHttpByJsonAsync<T>(string url, KeyValuePair<HTTPVerb, string> command , object? data)
		{
			HttpResponseMessage? response = null;
			string FinalURL = url + $"/{command.Value}";
			StringContent? content = null;
			if (data != null)
			{
				string jsonData = Newtonsoft.Json.JsonConvert.SerializeObject(data);
				content = new StringContent(jsonData, Encoding.UTF8, "application/json");
			}
			switch (command.Key)
			{
				case HTTPVerb.GET:
					{
						response = await client.GetAsync(FinalURL);
						break;
					}
				case HTTPVerb.POST:
					{
						response = await client.PostAsync(FinalURL, content);
						break;
					}
				case HTTPVerb.DELETE:
					{
						response = await client.DeleteAsync(FinalURL);
						break;
					}
				case HTTPVerb.PUT:
					{
						response = await client.PutAsync(FinalURL, content);
						break;
					}
				case HTTPVerb.PATCH:
					{
						response = await client.PatchAsync(FinalURL, content);
						break;
					}
			}

			if (response != null && response.StatusCode == System.Net.HttpStatusCode.OK)
			{
				string responseBody = await response.Content.ReadAsStringAsync();
				if (responseBody != null)
				{
					return Newtonsoft.Json.JsonConvert.DeserializeObject<T>(responseBody);
				}
			}

			return default(T);
		}
	}
}
