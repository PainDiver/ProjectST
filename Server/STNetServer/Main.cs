using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using STNetServer;

class TcpServer
{
	static async Task Main()
	{
		ServerCore Core = new ServerCore(17777);
		Core.StartServer();

		
	}

}