using STNetServer.Core;
using STNetServer.Core.NewFolder;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace STNetServer.Command
{

	internal class CommandBase
	{
		public static void StartCommand(ServerCore Instance,string command, params string[] args)
		{
			Type? type = null;
			switch(command)
			{
				case "viewallclient":
					{
						type = typeof(Command_ViewAllClient);
						break;
					}
				case "forcerundedi":
					{
						type = typeof(Command_ForceRunDedi);
						break;
					}
				case "forceexitdedi":
					{
						type = typeof(Command_ForceExitDedi);
						break;
					}
				case "exit":
					{
						type = typeof(Command_Exit);
						break;
					}
			}

			if (type != null)
			{
				object instance = Activator.CreateInstance(type);
				((CommandBase)instance).Execute(Instance,args);
			}
		}
		public virtual void Execute(ServerCore Instance, params string[] args) { }
	}

	internal class Command_ViewAllClient : CommandBase
	{
		public override void Execute(ServerCore Instance, params string[] args)
		{
			Console.WriteLine("연결된 클라이언트 ID");
			foreach (string ID in Instance.ConnectedClients.Keys)
			{
				Console.WriteLine(ID);
			}
		}
	}
	internal class Command_ForceRunDedi : CommandBase
	{
		public override void Execute(ServerCore Instance, params string[] args)
		{
			GameInstanceManager.Instance.RunAnyDedicateServer(out string batcherID, out string port);
		}
	}
	internal class Command_ForceExitDedi : CommandBase
	{
		public override void Execute(ServerCore Instance, params string[] args)
		{
			if(args.Length > 0)
				GameInstanceManager.Instance.EndDedicateServer(args[0]);
		}
	}
	internal class Command_Exit : CommandBase
	{
		public override void Execute(ServerCore Instance, params string[] args)
		{
			Instance.CloseServer();
		}
	}

	
}
