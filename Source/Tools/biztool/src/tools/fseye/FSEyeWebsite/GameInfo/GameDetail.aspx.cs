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
using FSEye;
using FSEye.Security;
using FSEye.FS2Lib;

public partial class GameInfo_GameDetail : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoGameDetail, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params["serverId"] != null)
		{
			if (!IsPostBack)
			{
				int serverId = 0;
				try
				{
					serverId = int.Parse(Request.Params["serverId"]);
				}
				catch (FormatException)
				{
				}
				m_server = TheAdminServer.GameServerManager.GetGameServer(serverId);
				if (m_server != null)
				{
					Session["SelectedGameServer"] = m_server;
				}
				else
				{
					Response.Redirect("GameInfo.aspx");
				}
			}
			else
			{
				if (Session["SelectedGameServer"] != null)
				{
					m_server = (GameServer)Session["SelectedGameServer"];
				}
				else
				{
					Response.Redirect("GameInfo.aspx");
				}
			}
		}
		else
		{
			Response.Redirect("GameInfo.aspx");
		}
	}

	GameServer m_server;

	public int GetGameServerId()
	{
		if (m_server != null)
		{
			return m_server.Id;
		}
		else
		{
			return 0;
		}
	}
}
