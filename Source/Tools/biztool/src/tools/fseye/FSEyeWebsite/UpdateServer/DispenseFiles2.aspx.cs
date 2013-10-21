using System;
using System.Timers;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Collections.Generic;
using System.Net;
using System.IO;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using Resources;
using FSEye;
using FSEye.Security;   //TODO:这个类没有序列化，需要重写 huangbiao 2008.3
using FSEye.PlugIn;
using JX2Lib;


public partial class UpdateServer_DispenseFiles2 : BasePage
{
    string SecurityObjectGameServerGroups = StringDef.AllServers;
    int _updateServerId = 0;
    static DateTime timeStart = new DateTime();
    
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerDispenseFiles2, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }     

        if (!Page.IsPostBack)
        {
            CreateServerGroupTree();
            
            //TreeViewServerGroup.Attributes.Add("onclick", "goPostBack()");

            TreeViewServerGroup.Attributes.Add("onclick", "OnTreeClick(event)");
        }
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
        TreeViewServerGroup.ExpandDepth = 1;
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
                 node.Value ="g" + group.Id.ToString();

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

    /*
    protected void TreeViewServerGroup_TreeNodeCheckChanged(object sender, TreeNodeEventArgs e)
    {
        SetChildChecked(e.Node);

        // 判断是否是根节点
        if (!e.Node.Value.Equals("0"))
        {
            OneChildUnCheckedSetParentUnChecked(e.Node);
        }
    }

    /// <summary>
    /// 根据父节点状态设置子节点的状态    
    /// 循环设置所有子节点状态为父节点状态
    /// </summary>
    /// <param name="node"></param>
    private void SetChildChecked(TreeNode node)
    {
        foreach (TreeNode childNode in node.ChildNodes)
        {
            childNode.Checked = node.Checked;
            if (childNode.ChildNodes.Count > 0)
            {
                SetChildChecked(childNode);
            }
        }
    }

    // <summary>
    /// 根据子节点状态设置父节点的状态
    /// 只要有一个子节点打勾就给上一级父节点打勾
    /// </summary>
    /// <param name="childNode"></param>
    private void OneChildCheckedSetParentChecked(TreeNode childNode)
    {
        TreeNode parentNode = childNode.Parent;
        if (!parentNode.Checked && childNode.Checked)
        {
            parentNode.Checked = true;
        }
        else if (parentNode.Checked && !childNode.Checked)
        {
            int checks = 0;
            foreach (TreeNode node in parentNode.ChildNodes)
            {
                if (node.Checked)
                {
                    checks++;
                    break;
                }
            }
            if (checks == 0)
            {
                parentNode.Checked = false;
                if (!parentNode.Value.Equals("0"))
                {
                    OneChildCheckedSetParentChecked(parentNode);
                }
            }
        }
    }

    /// <summary>
    /// 根据子节点状态设置父节点的状态
    /// 只要有一个子节点不打勾，上一级父节点就不打勾
    /// </summary>
    /// <param name="childNode"></param>
    private void OneChildUnCheckedSetParentUnChecked(TreeNode childNode)
    {
        TreeNode parentNode = childNode.Parent;
        if (!childNode.Checked)
        {
            parentNode.Checked = false;
        }
    }
    */

    //下载
    protected void buttonDownload_Click(object sender, EventArgs e)
    {
       
    }

    //启动
    protected void buttonStart_Click(object sender, EventArgs e)
    {
        string selectGroup = "";
        LabelOpMsg.Text = "";
       

        if (TreeViewServerGroup.CheckedNodes.Count == 0) LabelOpMsg.Text = StringDef.SelectNoGameServer;

        //排序
        foreach (TreeNode node in TreeViewServerGroup.CheckedNodes)
        {
            if (!node.Value.StartsWith("g"))
                continue;

            //记录选中的组
            selectGroup +=  ";" + node.Value.Substring(1) + ";";
                        
        }
        Session["TreeSelectGroup"] = selectGroup;
        Page.Response.Redirect("GroupStatus.aspx?" + WebConfig.ParamGroupId + "=" + selectGroup);
    }

    //关闭
    protected void buttonStop_Click(object sender, EventArgs e)
    {
    }

    //更新
    protected void buttonDispense_Click(object sender, EventArgs e)
    {
        string ErrMsg = "";
        LabelOpMsg.Text = "";
        timeStart = DateTime.Now;
        if (TreeViewServerGroup.CheckedNodes.Count == 0) LabelOpMsg.Text = StringDef.SelectNoGameServer;

        foreach (TreeNode node in TreeViewServerGroup.CheckedNodes)
        {
            if (node.Value.StartsWith("s"))
            {
                string ipAddr = ""; //记录所有执行过自动更新命令的服务器IP
                //叶子节点
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(int.Parse(node.Value.Substring(1)));
                //清空上次更新历史
                server.UpdateMsg.Clear();
                //相同IP的服务器只调用一次更新命令
                if (ipAddr.IndexOf(server.IpAddress)== -1)
                {
                    ipAddr += "," + server.IpAddress;
                    if (server.IsConnected)
                        autoUpdate(server, Request.Params["updatePak"].ToString());
                    else
                        ErrMsg += server.Name + " ";
                }
            }
        }

        if (ErrMsg.Trim() != string.Empty)
            LabelOpMsg.Text = ErrMsg + StringDef.NoConnectionAlert;
    }

    private void autoUpdate(GameServer server, string ftpPacket)
    {
        if (Page.Request.Params["serverId"] != null)
        {
            string[] para = new string[4];

            _updateServerId = int.Parse(Request.Params["serverId"].ToString());
            FTPServer ftpSvr = AdminServer.TheInstance.FTPClient.GetFTPServer(_updateServerId);
            ////需要对该UpdateServer有Read的权限
            //if (!WebUtil.CheckPrivilege(ftpServer.SecurityObject, OpType.READ, Session))
            //{
            //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            //}

            para[0] = ftpSvr.Address;
            para[1] = ftpSvr.UserName;
            para[2] = ftpSvr.Password;
            para[3] = ftpPacket;

            if (server.DoPlugInAction(CurrentUser.Id, E2gUpdateserver.PlugInGuid, E2gUpdateserver.actionKey, para))
            {
                LabelOpMsg.Text = StringDef.Success;
                LabelOpMsg.Visible = true;
            }
            else
            {
                LabelOpMsg.Text = StringDef.Failure;
            }
            para = null;
        }
        TimeNow.Text = StringDef.TimeNow + ":" + DateTime.Now.Hour.ToString() + ":" + DateTime.Now.Minute.ToString() + ":" + DateTime.Now.Second.ToString();

        TreeViewServerGroup.Enabled = false;
        Timer1.Enabled = true;        
    }

    protected void ListServerFiles()
    {
        bool bFinished = true;  //所有服务器上传完毕标志
        foreach (TreeNode node in TreeViewServerGroup.CheckedNodes)
        {
            if (node.Value.StartsWith("s"))
            {
                //叶子节点
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(int.Parse(node.Value.Substring(1)));

                TableHeaderRow headerServer = new TableHeaderRow();
                TableHeaderCell[] headServer = new TableHeaderCell[2];
                headServer[0] = new TableHeaderCell();
                headServer[0].Width = new Unit(10f, UnitType.Percentage);
                headServer[1] = new TableHeaderCell();
                headServer[1].Width = new Unit(30f, UnitType.Percentage);
                headServer[1].Text = server.Name;
                headerServer.Cells.AddRange(headServer);
                ServerFileList.Rows.Add(headerServer);


                TableHeaderRow header = new TableHeaderRow();                
                TableHeaderCell[] head = new TableHeaderCell[2];
                for (int i = 0; i <= 1; i++) head[i] = new TableHeaderCell();
                head[0].Width = new Unit(10f, UnitType.Percentage);
                head[0].Text = StringDef.Time;
                head[1].Width = new Unit(30f, UnitType.Percentage);
                head[1].Text = StringDef.Message;

                header.Cells.AddRange(head);
                ServerFileList.Rows.Add(header);

                


                if (server.UpdateMsg.Count == 0)
                {
                    if (server.IsConnected)
                    {
                        bFinished = false;
                        TableRow row = new TableRow();
                        TableCell[] cell = new TableCell[2];
                        for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                        cell[0].Text = server.Name;
                        cell[1].Text = StringDef.Running;
                        row.Cells.AddRange(cell);
                        ServerFileList.Rows.Add(row);
                    }
                    else
                    {
                        TableRow row = new TableRow();
                        TableCell[] cell = new TableCell[2];
                        for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                        cell[0].Text = server.Name;
                        cell[1].Text = StringDef.NoConnectionAlert;
                        cell[1].BackColor = System.Drawing.Color.Red;
                        row.Cells.AddRange(cell);
                        ServerFileList.Rows.Add(row);
                    }
                }
                else
                {
                    foreach (string[] str in server.UpdateMsg)
                    {
                        TableRow row = new TableRow();
                        TableCell[] cell = new TableCell[2];
                        for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                        cell[0].Text = str[4];//server.Name;
                        cell[1].Text = str[0] + "/" + str[1] + ":" + str[3];
                        if (str[2] == "0")
                        {
                            cell[1].BackColor = System.Drawing.Color.Red;
                        }

                        row.Cells.AddRange(cell);
                        ServerFileList.Rows.Add(row);
                    }

                    string[] strMsg = server.UpdateMsg[server.UpdateMsg.Count - 1] as string[];

                    if (strMsg[0] != strMsg[1] || strMsg[2] == "0")
                    {
                        //所有步骤执行完毕或执行出错
                        bFinished = false;                        
                    }
                }
            }
        }

        if (bFinished)
        {
            //停止TIMER
            Timer1.Enabled = false;
            TimeSpan timeCost = DateTime.Now - timeStart;
            ShowCostTime.Text = StringDef.CostTime + timeCost.Hours.ToString() + StringDef.Hour + ":" + timeCost.Minutes.ToString() + StringDef.Minute + ":" + timeCost.Seconds.ToString() + StringDef.Second;
        }

    }

    protected void Timer1_Tick1(object sender, EventArgs e)
    {
        ListServerFiles();
        TimeSpan timeCost = DateTime.Now - timeStart;
        ShowCostTime.Text = StringDef.CostTime + timeCost.Hours.ToString() + StringDef.Hour + ":" + timeCost.Minutes.ToString() + StringDef.Minute + ":" + timeCost.Seconds.ToString() + StringDef.Second;
    }
}
