using STNetUtils.REST;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetUtils.RESTContext
{
	using HTTPPair = KeyValuePair<HTTPVerb, string>;
	public class GameInstanceServerContext
	{
		public static readonly string MatchingURL = new string("http://localhost:5245/Matching/");

		//Commands
		public static readonly HTTPPair EnqueueClient = new HTTPPair(HTTPVerb.POST, "EnqueueClient");
		public static readonly HTTPPair ForceStartDedicateServer = new HTTPPair(HTTPVerb.POST, "ForceStartDedicateServer");
		public static readonly HTTPPair ShutdownDedicateServer = new HTTPPair(HTTPVerb.DELETE, "ShutdownDedicateServer");

	}
	public class MatchingClientInfo
	{
		public MatchingClientInfo(string name)
		{
			ClientID = name;
		}
		string? ClientID;
	}


}

