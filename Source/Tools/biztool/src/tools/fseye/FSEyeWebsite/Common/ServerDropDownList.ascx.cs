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

/// <summary>
/// 列出游戏服务器的一个用户控件ServerDropDownList
/// 有三个属性
/// </summary>
public partial class Common_ServerDropDownList : System.Web.UI.UserControl
{
    #region 属性
    //服务器数量
    public int ServerCount
    {
        get { return ServerList.Items.Count; }
    }

    //选择的服务器的ID
    public int SelectedServerId
    {
        get
		{
			try
			{
				return int.Parse(ServerList.SelectedValue);
			}
			catch (Exception)
			{
				return 0;
			}
		}
    }
    //选择的服务器(GameServer)
    public GameServer SelectedGameServer
    {
        get { return AdminServer.TheInstance.GameServerManager.GetGameServer(this.SelectedServerId); }
		set
		{
			if (value != null)
			{
				ListItemCollection items = ServerList.Items;
				for (int i = 0; i < items.Count; i++)
				{
					if (items[i].Value == value.Id.ToString())
					{
						ServerList.SelectedIndex = i;
						break;
					}
				}
			}
		}
    }

    public bool AutoPostBack
    {
        get { return ServerList.AutoPostBack; }
        set { ServerList.AutoPostBack = value; }
    }

    public ListItemCollection Items
    {
        get { return ServerList.Items; }        
    }

    public bool Enabled
    {
        get { return ServerList.Enabled; }
        set { ServerList.Enabled = value; }
    }

    #endregion

    #region 事件&代理
    public delegate void SelectedIndexChangedEventHandler(object sender, EventArgs e);
    public event SelectedIndexChangedEventHandler ServerIndexChanged;
    #endregion

    protected void Page_Init(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            //IList servers = AdminServer.TheInstance.GameServerManager.Servers;    
            //if (servers.Count > 0)
            //{
            //    //用于选择第一个有连接的服务器作为默认的选项
            //    bool noConnect = true;
            //    int firstSelected = 0;
            //    foreach (GameServer server in servers)
            //    {
            //        ServerList.Items.Add(new ListItem(server.Name, server.Id.ToString()));
            //        if (noConnect && server.IsConnected)
            //        {
            //            ServerList.SelectedIndex = firstSelected;
            //            noConnect = false;
            //        }
            //        ++firstSelected;
            //    }
            //    if (noConnect) ServerList.SelectedIndex = 0;
            //}
        }
    }

    protected void OnServerIndexChanged(object sender,EventArgs e)
    {
        if (ServerIndexChanged != null) ServerIndexChanged(this, e);
    }

    protected void SelectedIndexChanged_Click(object sender, EventArgs e)
    {
        this.OnServerIndexChanged(sender, e);
    }

    public void CreateServerList(ServerGroup serverGroup)
    {
        if (serverGroup != null)
        {
            ServerList.Items.Clear();
            FSEyeObject[] obj = AdminServer.TheInstance.SecurityManager.Get(serverGroup.Id).Children;
            if (obj.Length != 0)
            {
                bool SelecteConnect = false;
                foreach (FSEyeObject serverObj in obj)
                {
                    ListItem item = new ListItem(serverObj.Name, serverObj.Id.ToString());
                    ServerList.Items.Add(item);
                    if (!SelecteConnect)
                    {
                        GameServer server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverObj.Id);                        
                        if(server!=null && server.IsConnected)
                        {
                            item.Selected = true;
                            SelecteConnect = true;
                        }
                    }
                }
            }
        }
    }

    public void CreateServerList(ServerGroup serverGroup, GameServer.ServerType serverType)
    {        
            this.Items.Clear();
            foreach (GameServer server in serverGroup.List)
            {
                if (server.Type == serverType)
                {
                    ListItem item = new ListItem(server.Name, server.Id.ToString());
                    this.Items.Add(item);
                }
            }        
    }
}
