using System;
using System.Web;
using System.Collections;
using System.Web.Services;
using System.Web.Services.Protocols;
using FSEye;


/// <summary>
/// Summary description for TestService
/// </summary>
[WebService(Namespace = "http://tempuri.org/")]
[WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
public class TestService : System.Web.Services.WebService
{

	public TestService()
	{

		//Uncomment the following line if using designed components 
		//InitializeComponent(); 
	}

	[WebMethod]
	public string HelloWorld()
	{
		return "Hello World";
	}

	[Serializable]
	public class GameServerStatus
	{
		public string Name;

		public GameServer.ServerState ServerState;

		public GameServerStatus()
		{
		}

		public GameServerStatus(string name, GameServer.ServerState serverState)
		{
			Name = name;
			ServerState = serverState;
		}
	};

	[WebMethod]
	public GameServerStatus[] GetGameServerList()
	{
		ArrayList returnList = new ArrayList();
		IList gameServerList = AdminServer.TheInstance.GameServerManager.Servers;
		foreach(GameServer server in gameServerList)
		{
			returnList.Add(new GameServerStatus(server.Name, server.GameServerState));
		}
		return (GameServerStatus[])returnList.ToArray(typeof(GameServerStatus));
	}

}

