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
using System.Drawing;
using FSEye;
using FSEye.Security;

public partial class GameServer_ServerDetail : BasePage
{
	private const int MAX_CURRENT_TASK_TEXT_LENGTH = 5;

	GameServer m_server;

	public int GetGameServerId()
	{
		return m_server.Id;
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerDetail, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params["serverId"] != null)
		{
			if (!IsPostBack)
			{
				int serverId = int.Parse(Request.Params["serverId"]);
				m_server = TheAdminServer.GameServerManager.GetGameServer(serverId);
				if (m_server != null)
				{
					Session["SelectedGameServer"] = m_server;
				}
				else
				{
					Response.Redirect("GameServer.aspx");
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
					Response.Redirect("GameServer.aspx");
				}
			}
		}
		else
		{
			Response.Redirect("GameServer.aspx");
		}
	}
}
