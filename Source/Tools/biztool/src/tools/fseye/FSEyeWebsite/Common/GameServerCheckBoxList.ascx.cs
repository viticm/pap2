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
using FSEye;

public partial class Common_GameServerCheckBoxList : System.Web.UI.UserControl
{
	public GameServer[] SelectedGameServers
	{
		get
		{
			ArrayList selectedGameServerList = new ArrayList();
			GameServerManager gameServerManager = AdminServer.TheInstance.GameServerManager;
			foreach (ListItem serverItem in CheckBoxListGameServer.Items)
			{
				if (serverItem.Selected && serverItem.Enabled)
				{
					int serverId = int.Parse(serverItem.Value);
					GameServer server = gameServerManager.GetGameServer(serverId);
					selectedGameServerList.Add(server);
				}
			}

			return (GameServer[])selectedGameServerList.ToArray(typeof(GameServer));
		}
	}

	public string CssClass
	{
		get { return PanelBorder.CssClass; }
		set { PanelBorder.CssClass = value; }
	}

	public int RepeatColumns
	{
		get { return CheckBoxListGameServer.RepeatColumns; }
		set { CheckBoxListGameServer.RepeatColumns = value; }
	}

	public Unit GameServerPanelHeight
	{
		get { return PanelGameServer.Height; }
		set { PanelGameServer.Height = value; }
	}

	public void SetServerSelected(int serverId, bool isSelected)
	{
		string serverIdStr = serverId.ToString();
		foreach (ListItem item in CheckBoxListGameServer.Items)
		{
			if (item.Value == serverIdStr)
			{
				item.Selected = isSelected;
				break;
			}
		}
       
	}
    public void ShowsServerSelected()
    {
        string _serverSelected = "";
        foreach (ListItem item in CheckBoxListGameServer.Items)
        {
            if (item.Selected==true)
            {
                _serverSelected += item.Text;
            }
        }

        ServerSelectedSummary.Text = StringDef.ServerSelected+_serverSelected;

    }

	protected void Page_Init(object sender, EventArgs e)
	{
		if (!IsPostBack)
		{
			ListGameServers();
            
		}
	}

	protected void Page_Load(object sender, EventArgs e)
	{
	}

	protected void ServerFilter1_FilterChanged(object sender, EventArgs e)
	{
		IList servers = AdminServer.TheInstance.GameServerManager.Servers;
		IList matchServers = ServerFilter1.FilterGameServer(servers);

		ArrayList invalidItems = new ArrayList();
		foreach(ListItem item in CheckBoxListGameServer.Items)
		{
			int id = int.Parse(item.Value);
			GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(id);
			if (server != null)
			{
				item.Enabled = matchServers.Contains(server);
			}
			else
			{
				//这些服务器已经不存在了
				invalidItems.Add(item);
			}
		}

		//删除已经不存在的服务器
		foreach (ListItem item in invalidItems)
		{
			CheckBoxListGameServer.Items.Remove(item);
		}
		
		LabelSummary.Text = String.Format(StringDef.MatchServerCount, matchServers.Count);
	}

	private void ListGameServers()
	{
		IList servers = AdminServer.TheInstance.GameServerManager.Servers;
		if (servers.Count > 0)
		{
          
			LabelNoGameServer.Visible = false;
			foreach (GameServer server in servers)
			{
                string serverTypeChnName = Enum.GetName(typeof(GameServer.ServerType),(UInt16)server.Type);
                ListItem item = new ListItem("[" + server.Id.ToString() + "] " + server.Name + " - " + "[" + serverTypeChnName + "] " + "-" + (server.IsConnected ? StringDef.Connected : StringDef.Disconnected), server.Id.ToString());
				CheckBoxListGameServer.Items.Add(item);
               
			}
		}
		else
		{
			LabelNoGameServer.Visible = true;
		}

		LabelSummary.Text = String.Format(StringDef.MatchServerCount, servers.Count);


	}

	protected void ButtonSelectAllServers_Click(object sender, EventArgs e)
	{
		SetAllServers(true);
	}

	protected void ButtonSelectNoServer_Click(object sender, EventArgs e)
	{
		SetAllServers(false);
	}

	private void SetAllServers(bool select)
	{
		foreach (ListItem item in CheckBoxListGameServer.Items)
		{
			if (item.Enabled)
				item.Selected = select;
		}
	}
}
