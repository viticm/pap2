//////////////////////////////////////////////////////////////////////
//
//此页面用来执行高级GM指令，即由使用者自行输入指令内容
//因此，此页面需要严格控制权限，
//应当只有admin组和AdvancedGameMaster组才具有访问和执行权限
//2008.8.18
//
//////////////////////////////////////////////////////////////////////


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
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;
using FSEye.FS2Lib;

public partial class GameMaster_GMCommandAdvanced : BasePage
{
    string SecurityObjectGameServerGroups = StringDef.AllServers;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!IsPostBack)
        {
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterGMCommandAdvanced, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            InitializeListBox();

            CreateServerGroupTree();

            TreeViewServerGroup.Attributes.Add("onclick", "OnTreeClick(event)");
        }        

        //给该Blaze控件增加EventHandler
        serverGroupDropDownList.SelectedServerGroupChanged += new EventHandler(serverGroupDropDownList_SelectedServerGroupChanged);

    }

    private void InitializeListBox()
    {
        ListBoxOperation.Items.Add(new ListItem(StringDef.Custom));

        ListBoxOperation.SelectedIndex = 0;
        SetPanelVisible();

        TextBoxCustomCommand.Height = new Unit("280px");
        TextBoxCustomCommand.Width = new Unit("100%");
    }


    protected void serverGroupSelectionOptionRadioButtonList_OnSelectedIndexChanged(object sender, EventArgs e)
    {
        if (serverGroupSelectionOptionRadioButtonList.SelectedValue == "TreeView")
        {
            TreeViewServerGroup.Visible = true;
            serverGroupDropDownList.Visible = false;

            string selectedValue = GMTargetDropDownList.SelectedValue;
            ListItem gameServerListItem = GMTargetDropDownList.Items.FindByValue("GameServer");
            if (gameServerListItem != null)
                GMTargetDropDownList.Items.Remove(gameServerListItem);
            if (selectedValue == "GameServer")
            {
                GMTargetDropDownList.SelectedValue = "GameCenter";
                GMTargetDropDownList_SelectedIndexChanged(null, null);
            }
        }
        else
        {
            TreeViewServerGroup.Visible = false;
            serverGroupDropDownList.Visible = true;

            if (GMTargetDropDownList.Items.FindByValue("GameServer") == null)
            {
                string selectedValue = GMTargetDropDownList.SelectedValue;

                ListItem gameServerListItem = new ListItem(StringDef.ServerTypeGameServer, "GameServer");
                GMTargetDropDownList.Items.Insert(1, gameServerListItem);
                GMTargetDropDownList.SelectedValue = selectedValue;
            }
        }

    }

    protected void ListBoxOperation_IndexChanged(object sender, EventArgs e)
    {
        SetPanelVisible();
    }

    private void SetPanelVisible()
    {
        PanelCustom.Visible = true;
    }


    void CreateServerGroupTree()
    {
        IList topGroupList = TheAdminServer.GameServerManager.TopServerGroups;

        TreeViewServerGroup.Nodes.Clear();

        TreeNode rootNode = new TreeNode();
        rootNode.Text = SecurityObjectGameServerGroups;
        rootNode.Value = "0";



        foreach (ServerGroup group in topGroupList)
        {
            TreeNode node = CreateServerGroupNode(group);
            if (node != null)
            {
                rootNode.ChildNodes.Add(node);
            }
        }

        TreeViewServerGroup.Nodes.Add(rootNode);
        TreeViewServerGroup.ExpandDepth = 0;
    }

    TreeNode CreateServerGroupNode(ServerGroup group)
    {
        if (group != null)
        {
            if (group.List.Count == 0)
            {
                return null;
            }
            TreeNode node = new TreeNode();
            node.Text = group.Name;
            node.Value = group.Id.ToString();

            if (group.GroupType == ServerGroup.Type.Group)
            {
                foreach (ServerGroup childGroup in group.List)
                {
                    TreeNode childNode = CreateServerGroupNode(childGroup);
                    if (childNode != null)
                    {
                        node.ChildNodes.Add(childNode);
                    }
                }

            }
            else if (group.GroupType == ServerGroup.Type.Server)
            {
                node.NavigateUrl = "~/GameServer/ServerList.aspx?serverGroupId=" + node.Value;
                node.Value = "g" + group.Id.ToString();

                // 显示group和下面的server
                //foreach (GameServer childServer in group.List)
                //{
                //    TreeNode childNode = new TreeNode();
                //    childNode.Text = childServer.Name;
                //    childNode.Value = 's' + childServer.Id.ToString();
                //    if (childNode != null)
                //    {
                //        node.ChildNodes.Add(childNode);
                //    }
                //}
            }

            return node;
        }

        return null;
    }

    //后面的代码有了修改和新增
    protected void GMTargetDropDownList_SelectedIndexChanged(object sender, EventArgs e)
    {
        switch (GMTargetDropDownList.SelectedValue)
        {
            case "GameCenter":
                roleOrAccountTableRow.Visible = false;
                gameServersTableRow.Visible = false;
                break;
            case "GameServer":
                roleOrAccountTableRow.Visible = false;
                gameServersTableRow.Visible = true;
                getGameServersFromSelectedServerGroup();
                break;
            case "Player":
                roleOrAccountTableRow.Visible = true;
                gameServersTableRow.Visible = false;
                break;

            default:
                break;
        }
    }

    private void getGameServersFromSelectedServerGroup()
    {
        if (serverGroupDropDownList.SelectedValue == null)
            return;

        if (gameServersTableRow.Visible == true)
        {
            gameServersCheckBoxList.Items.Clear();
            IList selectedGroup = serverGroupDropDownList.SelectedServer.Group.List;
            foreach (GameServer server in selectedGroup)
            {
                if (server.Type == GameServer.ServerType.gameserver)
                {
                    ListItem item = new ListItem("[" + server.Id.ToString() + "] " + server.Name + " - " + (server.IsConnected ? StringDef.Connected : StringDef.Disconnected), server.Id.ToString());
                    gameServersCheckBoxList.Items.Add(item);
                }
            }
        }
    }

    void serverGroupDropDownList_SelectedServerGroupChanged(object sender, EventArgs e)
    {
        if (gameServersTableRow.Visible == true)
            getGameServersFromSelectedServerGroup();
    }


    protected void LinkButton_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterGMCommandAdvanced, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            string gmCmd = string.Empty;
            string roleName = string.Empty;
            bool isAccountName = false;
            bool isExecutedByGameServer = false;
            switch (lb.CommandName)
            {
                case "Custom":
                    if (GMTargetDropDownList.SelectedValue == "Player")
                    {
                        roleName = TextBoxCustomRoleName.Text.Trim();
                        if (roleOrAccountDropDownList.SelectedValue == "AccountName")
                            isAccountName = true;
                        if (roleName != String.Empty)
                            isExecutedByGameServer = true;
                    }
                    string cmd = TextBoxCustomCommand.Text;
                    if (cmd.Length == 0)
                    {
                        LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Command);
                    }
                    gmCmd = cmd;
                    break;
            }

            ArrayList serversToGetResultArrayList = new ArrayList();

            try
            {
                if (serverGroupSelectionOptionRadioButtonList.SelectedValue == "DropDownList")
                {
                    int serverId = serverGroupDropDownList.SelectedServer.Id;

                    if (GMUtil.ExeGMCommand(CurrentUser.Id, serverId, roleName, isAccountName, gmCmd, isExecutedByGameServer, null, true))
                    {
                        LabelOpMsg.Text = "GM指令发送成功";

                        serversToGetResultArrayList.Add(AdminServer.TheInstance.GameServerManager.GetGameServer(serverId));
                    }
                    else
                    {
                        LabelOpMsg.Text = "GM指令发送失败";
                        return;
                    }
                }
                else if (serverGroupSelectionOptionRadioButtonList.SelectedValue == "TreeView")
                {
                    LabelOpMsg.Text = String.Empty;

                    int totalTargetServer = 0;
                    int successCount = 0;

                    foreach (TreeNode checkedTreeNode in TreeViewServerGroup.CheckedNodes)
                    {
                        try
                        {
                            if (checkedTreeNode.Value.StartsWith("g"))
                            {
                                ServerGroup serverGroup = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(checkedTreeNode.Value.Substring(1)));
                                GameServer gameCenter = null;
                                foreach (GameServer gameServer in serverGroup.List)
                                {
                                    if (gameServer.Type == GameServer.ServerType.gamecenter)
                                    {
                                        gameCenter = gameServer;
                                        break;
                                    }
                                }
                                if (gameCenter != null)
                                {
                                    totalTargetServer++;

                                    if (GMUtil.ExeGMCommand(CurrentUser.Id, gameCenter.Id, roleName, isAccountName, gmCmd, isExecutedByGameServer, null, true))
                                    {
                                        serversToGetResultArrayList.Add(gameCenter);

                                        successCount++;
                                    }
                                }
                            }
                        }
                        catch (Exception exception)
                        {
                            Util.DebugLog(exception.ToString());
                        }
                    }

                    LabelOpMsg.Text = "发送指令: " + successCount.ToString() + "/" + totalTargetServer.ToString();
                }
            }
            catch (Exception ex)
            {
                Util.DebugLog(ex.ToString());
            }
            finally
            {
                LabelOpMsg.Visible = true;
                LabelResultReceivedMsg.Visible = false;

                if (serversToGetResultArrayList.Count != 0)
                {
                    Session["ServersToGetResult"] = serversToGetResultArrayList;

                    Session["ServersToGetResultTotalCount"] = serversToGetResultArrayList.Count;

                    Session["ResultReceivedCount"] = 0;

                    ((HtmlTextArea)(resultMessagePanel.FindControl("resultMessageTextArea"))).Value = string.Empty;

                    if (ShowGMCommandResultTimer.Enabled == false)
                        ShowGMCommandResultTimer.Enabled = true;
                }
            }
        }
    }

    protected void ShowGMCommandResult(object sender, EventArgs e)
    {
        if (Session["ServersToGetResult"] == null)
            return;

        int totalTargetServer = (int)Session["ServersToGetResultTotalCount"];

        ArrayList serversToGetResultArrayList = Session["ServersToGetResult"] as ArrayList;

        ArrayList newServersToGetResultArrayList = new ArrayList();

        foreach (GameServer server in serversToGetResultArrayList)
        {
            string serverResult = GMUtil.GetGMCommandResult(server, CurrentUser.Id);

            if (serverResult != null)
            {
                ((HtmlTextArea)(resultMessagePanel.FindControl("resultMessageTextArea"))).Value += server.Name + ":\t" + serverResult + "\n\n";
            }
            else
            {
                newServersToGetResultArrayList.Add(server);
            }
        }

        if (newServersToGetResultArrayList.Count == 0)
            ShowGMCommandResultTimer.Enabled = false;
        else
            Session["ServersToGetResult"] = newServersToGetResultArrayList;

        int receivedCount = (int)Session["ServersToGetResultTotalCount"] - newServersToGetResultArrayList.Count;
        Session["ResultReceivedCount"] = receivedCount;

        LabelResultReceivedMsg.Text = "结果返回: " + receivedCount.ToString() + "/" + totalTargetServer.ToString();
        LabelResultReceivedMsg.Visible = true;
    }
}