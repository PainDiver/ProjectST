using Grpc.Core;
using Microsoft.AspNetCore.Hosting.Server;
using STNetGameInstanceBatcher;
using STNet_GameInstanceBatcher_Server;
using System.Diagnostics;
using System.Net;
using System.Reflection;

namespace STNetGameInstanceBatcher.Services
{
	public class MatchingService : Match.MatchBase
	{
		private readonly ILogger<MatchingService> _logger;
		public MatchingService(ILogger<MatchingService> logger)
		{
			_logger = logger;
		}

		public override Task<DedicateServerInfo> RunDedicateServer(DedicateServerParam request, ServerCallContext context)
		{
			DedicateServerInfo instance = DedicateServerInstanceManager.Instance.RunDedicateServerInstance(request.BatcherID,request.Port);
			return Task.FromResult<DedicateServerInfo>(instance);
		}


		public override Task<RPCVoid> OnConnectInitially(RPCVoid param, ServerCallContext context)
		{
			Console.WriteLine("gRPC Test Done");
			return Task.FromResult<RPCVoid>(new RPCVoid());
		}

	}
}
