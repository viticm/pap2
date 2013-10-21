using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using Resources;
using FSEye.Security;
using FSEye;

public partial class GameServer_DeleteServer : BasePage
{
	private int _gameServerId;

	public string GameServerId
	{
		get
		{
			if (_gameServerId > 0)
			{
				return _gameServerId.ToString();
			}
			else
			{
				return string.Empty;
			}
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerDeleteServer, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params["serverId"] != null)
		{
			try
			{
				_gameServerId = int.Parse(Request.Params["serverId"]);
			}
			catch (Exception)
			{
			}

			if (_gameServerId > 0)
			{
				if (Request.Params["noPromot"] != null)
				{
					try
					{
						bool noPromot = bool.Parse(Request.Params["noPromot"]);
						if (noPromot)
						{
							DeleteGameServer(_gameServerId);
						}
					}
					catch (Exception)
					{
					}
				}
			}
		}

        LabelOpMsg.Text = string.Empty;
	}

	private void DeleteGameServer(int gameServerId)
	{		
		GameServer server = TheAdminServer.GameServerManager.GetGameServer(gameServerId);
		if (server != null)
		{
			if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
			{
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
			}

			TheAdminServer.GameServerManager.DeleteGameServer(server);
		}
	}
}
