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
using System.Text;

public partial class GameServer_ServerGroupManagement : BasePage
{
    string SecurityObjectGameServerGroups = "GameServerGroups";
    string SecurityObjectGameServerList = "/FSEye/System/GameServerManager/GameServerList";
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerGroupManagement, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
        
		if (!IsPostBack)
		{
			CreateServerGroupTree();

            ListItem item = new ListItem("未设置", "未设置");
            DropDownListCabinet.Items.Add(item);
            foreach (string cabinetName in AdminServer.TheInstance.GameServerManager.CabinetList)
            {
                if (cabinetName != string.Empty)
                    DropDownListCabinet.Items.Add(cabinetName);
            }
		}
        if (Request.Params[WebConfig.ParamAddGroup] == "addGroup")
        {
            ButtonReturntoAddServer.Visible = true;
        }
        else
        {
            ButtonReturntoAddServer.Visible = false;
        }

		ServerGroupDropDownList.GroupType = ServerGroup.Type.Server;

        TreeNode selNode = TreeViewServerGroup.SelectedNode;
        if (selNode != null)
        {
            LiteralSelectedNode.Text = GetSelectedNodeFullPath(selNode);
        }
        else
        {
            LiteralSelectedNode.Text = StringDef.None;
        }

        LabelSuccess.Visible = false;
        LabelOpMsg.Visible = false;
	}

    private String GetSelectedNodeFullPath(TreeNode node)
    {
        StringBuilder path = new StringBuilder();
        path.Append(node.Text);
        TreeNode parent = node.Parent;

        if (parent == null)
        {
            path.Insert(0, SecurityManager.ObjectPathDelimiter);
        }
        else
        {
            while (parent != null)
            {
                if (parent.Text == SecurityObjectGameServerGroups)
                {
                    break;
                }
                else
                {
                    path.Insert(0, SecurityManager.ObjectPathDelimiter);
                    path.Insert(0, parent.Text);

                    parent = parent.Parent;
                }
            }
        }

        return path.ToString();
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
				rootNode.ChildNodes.Add(node);
		}

		TreeViewServerGroup.Nodes.Add(rootNode);
        TreeViewServerGroup.ExpandDepth = 2;
	}

	TreeNode CreateServerGroupNode(ServerGroup group)
	{
		if (group != null)
		{
			TreeNode node = new TreeNode();
			node.Text = group.Name;
			node.Value = 'g' + group.Id.ToString();
			if (group.GroupType == ServerGroup.Type.Group)
			{
				foreach (ServerGroup childGroup in group.List)
				{
					TreeNode childNode = CreateServerGroupNode(childGroup);
					if (childNode != null)
						node.ChildNodes.Add(childNode);
				}
			}
			else if (group.GroupType == ServerGroup.Type.Server)
			{
				foreach (GameServer childServer in group.List)
				{
					TreeNode childNode = new TreeNode();
					childNode.Text = childServer.Name;
					childNode.Value = 's' + childServer.Id.ToString();
					if (childNode != null)
						node.ChildNodes.Add(childNode);
				}
			}

			return node;
		}

		return null;
	}

	object GetSelectedObject()
	{
		try
		{
			string selectedText = TreeViewServerGroup.SelectedValue;
            if (selectedText.Length == 1)
            {
                FSEyeObject fsObject = TheAdminServer.SecurityManager.Get(SecurityObjectGameServerList);
                return fsObject;
                    
            }
            else if (selectedText.Length > 1)
			{
				if (selectedText[0] == 's')
				{
					int serverId = int.Parse(selectedText.Substring(1));
					GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverId);
					return server;
				}
				else if (selectedText[0] == 'g')
				{
					int groupId = int.Parse(selectedText.Substring(1));
					ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(groupId);
					return group;
				}
			}
		}
		catch (Exception)
		{
		}

		return null;
	}

	protected void ButtonCreateServerGroup_Click(object sender, EventArgs e)
	{
        //需要在GameServerManager上有Write的权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        string serverGroupName = TextBoxServerGroupName.Text.Trim();
        if (serverGroupName.Length == 0)
        {
            LabelOpMsg.Text = "服务器组名不能为空";
            LabelOpMsg.Visible = true;
            return;
        }

        string serverGroupComment = TextBoxServerGroupComment.Text.Trim();
        if (serverGroupComment == String.Empty)
        {
            serverGroupComment = "服务器组添加时间：" + DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");
        }
        
        string serverGroupCabinet;
        if (DropDownListCabinet.SelectedIndex == 0)
            serverGroupCabinet = String.Empty;
        else
            serverGroupCabinet = DropDownListCabinet.SelectedItem.Text;

        try
        {
            ServerGroup parentGroup = null;
            object selectedObj = GetSelectedObject();
            if (selectedObj != null)
            {
                if (selectedObj.GetType() == typeof(ServerGroup))
                {
                    parentGroup = selectedObj as ServerGroup;
                    TheAdminServer.GameServerManager.AddServerGroup(
                            serverGroupName,
                            serverGroupComment,
                            ServerGroup.Type.Group,
                            parentGroup,
                            serverGroupCabinet);
                    ServerGroupDropDownList.Refresh();
                    CreateServerGroupTree();
                }
                else
                {
                    FSEyeObject fsObject = selectedObj as FSEyeObject;
                    if (fsObject != null && fsObject.FullPath.Equals(SecurityObjectGameServerList))
                    {
                        TheAdminServer.GameServerManager.AddServerGroup(
                                serverGroupName,
                                serverGroupComment,
                                ServerGroup.Type.Group,
                                parentGroup,                        
                                serverGroupCabinet);
                        ServerGroupDropDownList.Refresh();
                        CreateServerGroupTree();
                    }
                }

                //if (TextBoxCabinet.Text.Trim() != string.Empty)
                //{
                //    if (!AdminServer.TheInstance.GameServerManager.CabinetList.Contains(TextBoxCabinet.Text.Trim()))
                //    {
                //        AdminServer.TheInstance.GameServerManager.CabinetList.Add(TextBoxCabinet.Text.Trim());
                //    }
                //}

                LabelSuccess.Text = StringDef.OperationSucceed + StringDef.Colon + "服务器组 " + serverGroupName + " 添加成功";
                LabelSuccess.Visible = true;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
            return;
        }
	}

	protected void ButtonDeleteServerGroup_Click(object sender, EventArgs e)
	{
        //需要在GameServerManager上有Write的权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        try
        {
            object selectedObj = GetSelectedObject();
            if (selectedObj != null && selectedObj.GetType() == typeof(ServerGroup))
            {
                ServerGroup group = selectedObj as ServerGroup;
                string serverGroupName = group.Name;

                //此段代码为额外的检验代码
                //防止客户端意外传回错误数据导致误删除
                if (group.List.Count != 0)
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + "服务器组 " + serverGroupName + " 不为空，不允许删除操作";
                    LabelOpMsg.Visible = true;
                    return;
                }

                TheAdminServer.GameServerManager.DeleteServerGroup(group.Id);
                ServerGroupDropDownList.Refresh();
                CreateServerGroupTree();

                LabelSuccess.Text = StringDef.OperationSucceed + StringDef.Colon + "服务器组 " + serverGroupName + " 删除成功";
                LabelSuccess.Visible = true;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
            return;
        }
	}
	
	protected void ButtonMoveToGroup_Click(object sender, EventArgs e)
	{
        //需要在GameServerManager上有Write的权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }
        
        object selectedObj = GetSelectedObject();
		if (selectedObj != null && selectedObj.GetType() == typeof(GameServer))
		{
            try
            {
                GameServer server = selectedObj as GameServer;
                ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;

                TheAdminServer.GameServerManager.MoveToServerGroup(server, group);
                ServerGroupDropDownList.Refresh();
                CreateServerGroupTree();

                LabelSuccess.Text = StringDef.OperationSucceed + StringDef.Colon + "服务器 " + server.Name + " 成功移动到组 " + group.Name;
                LabelSuccess.Visible = true;
            }
            catch (Exception ex)
            {
                LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
                LabelOpMsg.Visible = true;
                return;
            }
		}
	}

	protected void ButtonDeleteServer_Click(object sender, EventArgs e)
	{
		object selectedObj = GetSelectedObject();
		if (selectedObj != null && selectedObj.GetType() == typeof(GameServer))
		{
			GameServer server = selectedObj as GameServer;
			Response.Redirect(string.Format("DeleteServer.aspx?serverId={0}", server.Id));
		}
	}

	protected void TreeViewServerGroup_SelectedNodeChanged(object sender, EventArgs e)
	{
		PanelCreateSeverGroup.Enabled = false;
		PanelDeleteServerGroup.Enabled = false;
		PanelMoveToGroup.Enabled = false;

		TreeNode selectedNode = TreeViewServerGroup.SelectedNode;
		if (selectedNode != null)
		{
			object selectedObj = GetSelectedObject();
			if (selectedObj != null)
			{
				if (selectedObj.GetType() == typeof(GameServer))
				{
					GameServer server = selectedObj as GameServer;
					ServerGroupDropDownList.SelectedServerGroup = server.Group;
                    TextBoxServerGroupName.Text = server.Group.Name;
                    TextBoxServerGroupComment.Text = server.Group.Comment;

                    if (server.Group.Cabinet == String.Empty)
                        DropDownListCabinet.SelectedIndex = 0;
                    else
                    {
                        ListItem item = DropDownListCabinet.Items.FindByText(server.Group.Cabinet);
                        if (item == null)
                            DropDownListCabinet.SelectedIndex = 0;
                        else
                            DropDownListCabinet.SelectedIndex = DropDownListCabinet.Items.IndexOf(item);
                    }
                    					
                    PanelMoveToGroup.Enabled = true;
                    PanelModifyServerGroup.Enabled = false;
				}
				else if (selectedObj.GetType() == typeof(ServerGroup))
				{
					ServerGroup group = selectedObj as ServerGroup;
					TextBoxServerGroupName.Text = group.Name;
					TextBoxServerGroupComment.Text = group.Comment;

                    if (group.Cabinet == String.Empty)
                        DropDownListCabinet.SelectedIndex = 0;
                    else
                    {
                        ListItem item = DropDownListCabinet.Items.FindByText(group.Cabinet);
                        if (item == null)
                            DropDownListCabinet.SelectedIndex = 0;
                        else
                            DropDownListCabinet.SelectedIndex = DropDownListCabinet.Items.IndexOf(item);
                    }
                    
                    PanelModifyServerGroup.Enabled = true;
                    PanelCreateSeverGroup.Enabled = true;
                    ButtonCreateServerGroup.Enabled = true;
                    if(group.GroupType == ServerGroup.Type.Server)
                    {
                        ButtonCreateServerGroup.Enabled = false;
                    }
                    if (group.List.Count == 0)
                    {
                        PanelDeleteServerGroup.Enabled = true;                     
                    }
				}
                else
                {
                    PanelCreateSeverGroup.Enabled = true;
                    ButtonCreateServerGroup.Enabled = true;
                    PanelModifyServerGroup.Enabled = false;
                }
			}
		}
	}

    protected void ButtonModifyServerGroup_Click(object sender, EventArgs e)
    {
        //需要在GameServerManager上有Write的权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        ServerGroup group = GetSelectedObject() as ServerGroup;
        if (group != null)
        {
            string serverGroupName = TextBoxServerGroupName.Text.Trim();
            if (serverGroupName.Length == 0)
            {
                LabelOpMsg.Text = "服务器组名不能为空";
                LabelOpMsg.Visible = true;
                return;
            }

            string serverGroupComment = TextBoxServerGroupComment.Text.Trim();

            string serverGroupCabinet;
            if (DropDownListCabinet.SelectedIndex == 0)
                serverGroupCabinet = String.Empty;
            else
                serverGroupCabinet = DropDownListCabinet.SelectedItem.Text;

            group.SecurityObject.Name = serverGroupName;
            group.SecurityObject.Comment = serverGroupComment;

            try
            {
                TheAdminServer.SecurityManager.Set(group.SecurityObject);
                CreateServerGroupTree();

                group.SetGroupCabinet(serverGroupCabinet);

                //if (serverGroupCabinet != string.Empty)
                //{
                //    if (!AdminServer.TheInstance.GameServerManager.CabinetList.Contains(serverGroupCabinet))
                //    {
                //        AdminServer.TheInstance.GameServerManager.CabinetList.Add(serverGroupCabinet);
                //    }
                //}

                LabelSuccess.Text = StringDef.OperationSucceed + StringDef.Colon + "服务器组 " + serverGroupName + " 修改成功";
                LabelSuccess.Visible = true;
            }
            catch (Exception ex)
            {
                LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
                LabelOpMsg.Visible = true;
                return;
            }
        }
    }
}
