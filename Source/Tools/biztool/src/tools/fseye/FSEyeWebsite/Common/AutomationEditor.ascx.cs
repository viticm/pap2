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
using System.Collections.Generic;
using Resources;
using FSEye;

public partial class Common_AutomationEditor : System.Web.UI.UserControl
{
	IAutomation _automation;

	bool _autoExpand;

	/// <summary>
	/// 自动化对象
	/// </summary>
	public IAutomation Automation
	{
		get
		{
			return _automation;
		}

		set
		{
			_automation = value;
			if (_automation != null)
			{
				Session[WebConfig.SessionAutomation] = _automation;
                SetParameter(_automation);
                LoadParameter(_automation);
                LitearlAutomationDes.Text = _automation.Description;
			}
			else
			{
				Session.Remove(WebConfig.SessionAutomation);
			}
			TreeViewAutomation.Nodes.Clear();
			TreeNode node = CreateAutomationNode(_automation, "Start: ", string.Empty);
			TreeViewAutomation.Nodes.Add(node);			
			node.Select();
			if (_autoExpand)
			{
				TreeViewAutomation.ExpandAll();
			}

			TreeViewAutomation_SelectedNodeChanged(null, null);
		}
	}

	/// <summary>
	/// 自动展开
	/// </summary>
	public bool AutoExpand
	{
		get { return _autoExpand; }
		set { _autoExpand = value; }
	}

	protected void Page_Init(object sender, EventArgs e)
	{
		if (!IsPostBack)
		{
			ListAutomation();
            CreateCheckBoxListServer();
            CreateCheckBoxListFtpServer();
            CreateCheckBoxListServerGroup();
		}        
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (Session[WebConfig.SessionAutomation] != null)
		{
			_automation = Session[WebConfig.SessionAutomation] as IAutomation;
		}
        //隐藏错误提示
        LabelParameterInfo.Visible = false;
	}

	private void ListAutomation()
	{
		Type[] automationTypes = AdminServer.TheInstance.AutomationManager.Automations;
		foreach (Type type in automationTypes)
		{
            IAutomation automation = Activator.CreateInstance(type) as IAutomation;
			DropDownListAutomation.Items.Add(new ListItem(automation.Name, automation.Guid));
		}
	}

	private TreeNode CreateAutomationNode(IAutomation automation, string title, string path)
	{
		TreeNode node = new TreeNode();

		if (automation != null)
		{
			node.Text = title + automation.ToString();
			node.Value = path;
						
			node.ChildNodes.Add(CreateAutomationNode(automation.Success, "success: ", "1"));
			
			node.ChildNodes.Add(CreateAutomationNode(automation.Failure, "failure: ", "0"));
			
			node.ChildNodes.Add(CreateAutomationNode(automation.Complete, "complete: ", "2"));			
		}
		else
		{
			node.Text = title + "--";
			node.Value = path;
		}

		return node;
	}

	protected void ButtonApply_Click(object sender, EventArgs e)
	{
		try
		{
			TreeNode node = TreeViewAutomation.SelectedNode;

			if (node != null)
			{
				string guid = DropDownListAutomation.SelectedValue;
				//string param = TextBoxParameter.Text;
				string title = string.Empty;
				string lastPathName = string.Empty;

				IAutomation automation = AdminServer.TheInstance.AutomationManager.Create(guid);

                if ((int)automation.Parameter[0] != 0)
                {
                    object[] paraObj = new object[(int)automation.Parameter[0] + 1];

                    switch ((int)automation.Parameter[0])
                    {
                        case 0:
                            break;
                        case 1:
                            paraObj[0] = 1;
                            paraObj[1] = TextBoxParameter1.Text.Clone();
                            break;
                        case 2:
                            paraObj[0] = 2;
                            paraObj[1] = TextBoxParameter1.Text.Clone();
                            paraObj[2] = TextBoxParameter2.Text.Clone();
                            break;
                        case 3:
                            paraObj[0] = 3;
                            paraObj[1] = TextBoxParameter1.Text.Clone();
                            paraObj[2] = TextBoxParameter2.Text.Clone();

                            //如果该任务的类型为“发邮件”
                            if (automation.Guid == "{eca2ad62-0238-4996-82c0-852009463d94}")
                            {
                                string tempString = String.Empty;
                                foreach (ListItem item in MailContentTypeCheckBoxList.Items)
                                {
                                    if (item.Selected == true)
                                    {
                                        tempString += item.Value;
                                        tempString += " ";
                                    }
                                }

                                paraObj[3] = tempString.Trim();
                            }
                            else
                            {
                                paraObj[3] = TextBoxParameter3.Text.Clone();
                            }

                            break;
                    }

                    automation.Parameter = paraObj;
                }

                //paraObj[paraIndex] = TextBoxParameter[paraIndex].Text.Clone();
                
                //automation.Parameter = paraObj;// param;

				if (_automation != null)
				{
					string pathString = node.ValuePath;
					string[] pathArray = pathString.Split('/');

					IAutomation parentAutomation = null;
					IAutomation selectedAutomation = _automation;
					foreach (string pathName in pathArray)
					{
						if (pathName.Length > 0)
						{
							lastPathName = pathName;
							parentAutomation = selectedAutomation;
							switch (pathName)
							{
								case "0":
									selectedAutomation = selectedAutomation.Failure;
									break;
								case "1":
									selectedAutomation = selectedAutomation.Success;
									break;
								case "2":
									selectedAutomation = selectedAutomation.Complete;
									break;
							}
						}
					}

					if (parentAutomation != null)
					{
						switch (lastPathName)
						{
							case "0":
								title = "failure: ";
								parentAutomation.Failure = automation;
								break;
							case "1":
								title = "success: ";
								parentAutomation.Success = automation;
								break;
							case "2":
								title = "complete: ";
								parentAutomation.Complete = automation;
								break;
						}
					}
					else
					{
						Automation = automation;
					}
				}
				else
				{
					Automation = automation;
				}

				TreeNodeCollection nodes = null;
				TreeNode parentNode = node.Parent;
				if (parentNode != null)
					nodes = parentNode.ChildNodes;
				else
					nodes = TreeViewAutomation.Nodes;

				int index = nodes.IndexOf(node);
				if (index >= 0)
				{
					nodes.Remove(node);
					nodes.AddAt(index, CreateAutomationNode(automation, title, lastPathName));
				}

				if (_autoExpand)
				{
					TreeViewAutomation.ExpandAll();
				}
			}

			LabelParameterInfo.Text = string.Empty;
			LabelParameterInfo.Visible = false;
		}
		catch (BadParameterException ex)
		{
			LabelParameterInfo.Text = ex.Message;
			LabelParameterInfo.Visible = true;
			//TextBoxParameter.Focus();
		}
	}

	protected void ButtonRemove_Click(object sender, EventArgs e)
	{
		TreeNode node = TreeViewAutomation.SelectedNode;

		if (node != null)
		{
			string pathString = node.ValuePath;
			string[] pathArray = pathString.Split('/');

			IAutomation parentAutomation = null;
			string lastPathName = string.Empty;
			IAutomation selectedAutomation = _automation;
			foreach (string pathName in pathArray)
			{
				if (pathName.Length > 0)
				{
					lastPathName = pathName;
					parentAutomation = selectedAutomation;
					switch (pathName)
					{
						case "0":
							selectedAutomation = selectedAutomation.Failure;
							break;
						case "1":
							selectedAutomation = selectedAutomation.Success;
							break;
						case "2":
							selectedAutomation = selectedAutomation.Complete;
							break;
					}
				}
			}

			string title = string.Empty;
			if (parentAutomation != null)
			{
				switch (lastPathName)
				{
					case "0":
						title = "failure: ";
						parentAutomation.Failure = null;
						break;
					case "1":
						title = "success: ";
						parentAutomation.Success = null;
						break;
					case "2":
						title = "complete: ";
						parentAutomation.Complete = null;
						break;
				}
			}
			else
			{
                title = "Start: ";
			}

			TreeNode parentNode = node.Parent;
			if (parentNode != null)
			{
				TreeNodeCollection nodes = parentNode.ChildNodes;
				int index = nodes.IndexOf(node);
				nodes.Remove(node);
				nodes.AddAt(index, CreateAutomationNode(null, title, lastPathName));
			}
			else
			{
				this.Automation = null;
			}
		}
	}
	
	protected void TreeViewAutomation_SelectedNodeChanged(object sender, EventArgs e)
	{
		TreeNode node = TreeViewAutomation.SelectedNode;

		string pathString = node.ValuePath;
		string[] pathArray = pathString.Split('/');

		IAutomation parentAutomation = null;
		string lastPathName = null;
		IAutomation selectedAutomation = _automation;
		foreach (string pathName in pathArray)
		{
			lastPathName = pathName;
			parentAutomation = selectedAutomation;
			switch (pathName)
			{
				case "0":
					selectedAutomation = selectedAutomation.Failure;
					break;
				case "1":
					selectedAutomation = selectedAutomation.Success;
					break;
				case "2":
					selectedAutomation = selectedAutomation.Complete;
					break;
			}
		}

		if (selectedAutomation != null)
		{
			int count = DropDownListAutomation.Items.Count;
			for (int i = 0; i < count; i++)
			{
				ListItem item = DropDownListAutomation.Items[i];
				if (item.Value == selectedAutomation.Guid)
				{
					DropDownListAutomation.SelectedIndex = i;
					break;
				}
			}

			//TextBoxParameter.Text = selectedAutomation.Parameter;
			LinkButtonRemove.Enabled = true;
            SetParameter(selectedAutomation);
            LoadParameter(selectedAutomation);
		}
		else
		{
			//TextBoxParameter.Text = string.Empty;
			LinkButtonRemove.Enabled = false;
            PanelParameterOut.Visible = false;
            DropDownListAutomation.SelectedIndex = 0;
            DropDownListAutomation_SelectedIndexChanged(null, null);
		}

        ResetServerListAndButton();
	}

    protected void DropDownListAutomation_SelectedIndexChanged(object sender, EventArgs e)
    {
        IAutomation automation = AdminServer.TheInstance.AutomationManager.Create(DropDownListAutomation.SelectedValue);
        LitearlAutomationDes.Text = automation.Description;
        PanelServer.Visible = false;
        PanelFtpServer.Visible = false;
        PanelServerGroup.Visible = false;
        TextBoxParameter1.Text = string.Empty;
        TextBoxParameter2.Text = string.Empty;
        TextBoxParameter3.Text = string.Empty;

        SetParameter(automation);
        ResetServerListAndButton();
    }

    protected void ButtonServerIdSelect_Click(object sender, EventArgs e)
    {
        Button btn = sender as Button;
        if (btn != null)
        {
            if (btn.Text.Equals(StringDef.Select))
            {
                LoadServerState(TextBoxParameter1.Text);
                PanelServer.Visible = true;
                btn.Text = StringDef.Hide;                
            }
            else
            {
                PanelServer.Visible = false;
                btn.Text = StringDef.Select;
            }
        }
    }

    protected void ButtonFtpServerIdSelect_Click(object sender, EventArgs e)
    {
        Button btn = sender as Button;
        if (btn != null)
        {
            if (btn.Text.Equals(StringDef.Select))
            {
                LoadFtpServerState(TextBoxParameter1.Text);
                PanelFtpServer.Visible = true;
                btn.Text = StringDef.Hide;
            }
            else
            {
                PanelFtpServer.Visible = false;
                btn.Text = StringDef.Select;
            }
        }
    }

    protected void ButtonServerGroupIdSelect_Click(object sender, EventArgs e)
    {
        Button btn = sender as Button;
        if (btn != null)
        {
            if (btn.Text.Equals(StringDef.Select))
            {
                LoadServerGroupState(TextBoxParameter2.Text);
                PanelServerGroup.Visible = true;
                btn.Text = StringDef.Hide;
            }
            else
            {
                PanelServerGroup.Visible = false;
                btn.Text = StringDef.Select;
            }
        }
    }

    protected void CheckBoxListServer_SelectedIndexChanged(object sender, EventArgs e)
    {
        StringBuilder serverText = new StringBuilder();
        foreach (ListItem item in CheckBoxListServer.Items)
        {
            if (item.Selected)
            {
                serverText.Append(item.Value);
                serverText.Append(' ');
            }
        }
        if (serverText.Length != 0) TextBoxParameter1.Text = serverText.ToString(0, serverText.Length - 1);
        else TextBoxParameter1.Text = string.Empty;
    }

    protected void CheckBoxListFtpServer_SelectedIndexChanged(object sender, EventArgs e)
    {
        StringBuilder serverText = new StringBuilder();
        foreach (ListItem item in CheckBoxListFtpServer.Items)
        {
            if (item.Selected)
            {
                serverText.Append(item.Value);
                serverText.Append(' ');
            }
        }
        if (serverText.Length != 0) TextBoxParameter1.Text = serverText.ToString(0, serverText.Length - 1);
        else TextBoxParameter1.Text = string.Empty;
    }

    protected void CheckBoxListServerGroup_SelectedIndexChanged(object sender, EventArgs e)
    {
        StringBuilder serverGroupText = new StringBuilder();
        foreach (ListItem item in CheckBoxListServerGroup.Items)
        {
            if (item.Selected)
            {
                serverGroupText.Append(item.Value);
                serverGroupText.Append(' ');
            }
        }
        if (serverGroupText.Length != 0) TextBoxParameter2.Text = serverGroupText.ToString(0, serverGroupText.Length - 1);
        else TextBoxParameter2.Text = string.Empty;
    }

    void SetParameter(IAutomation automation)
    {
        ButtonServerIdSelect.Visible = false;
        ButtonFtpServerIdSelect.Visible = false;
        ButtonServerGroupIdSelect.Visible = false;
        switch ((int)automation.Parameter[0])
        {
            case 0:
                PanelParameterOut.Visible = false;
                break;
            case 1:
                PanelParameterOut.Visible = true;
                if (automation.ParameterDes[0].Equals(StringDef.ServerId))
                    ButtonServerIdSelect.Visible = true;
                TextBoxParameter1.Visible = true;
                LabelParameter1.Text = automation.ParameterDes[0];
                LabelParameter1.Visible = true;
                TextBoxParameter2.Visible = false;
                LabelParameter2.Visible = false;
                TextBoxParameter3.Visible = false;
                LabelParameter3.Visible = false;

                MailContentTypeCheckBoxList.Visible = false;

                break;
            case 2:
                PanelParameterOut.Visible = true;

                TextBoxParameter1.Visible = true;
                if (automation.ParameterDes[0].Equals(StringDef.ServerId))
                    ButtonServerIdSelect.Visible = true;
                if (automation.Guid == "{2A034C09-D437-4b6a-8338-B76AA8F5D543}")
                    ButtonFtpServerIdSelect.Visible = true;
                LabelParameter1.Text = automation.ParameterDes[0];
                LabelParameter1.Visible = true;

                if (automation.ParameterDes[1].Equals(StringDef.ServerGroupId))
                    ButtonServerGroupIdSelect.Visible = true;
                TextBoxParameter2.Visible = true;
                LabelParameter2.Text = automation.ParameterDes[1];
                LabelParameter2.Visible = true;
                TextBoxParameter3.Visible = false;
                LabelParameter3.Visible = false;

                MailContentTypeCheckBoxList.Visible = false;

                break;
            case 3:
                //如果该任务的类型为“发邮件”
                if (automation.Guid == "{eca2ad62-0238-4996-82c0-852009463d94}")
                {
                    PanelParameterOut.Visible = true;
                    if (automation.ParameterDes[0].Equals(StringDef.ServerId))
                        ButtonServerIdSelect.Visible = true;
                    TextBoxParameter1.Visible = true;
                    LabelParameter1.Visible = true;
                    LabelParameter1.Text = automation.ParameterDes[0];

                    if (automation.ParameterDes[1].Equals(StringDef.ServerGroupId))
                        ButtonServerGroupIdSelect.Visible = true;
                    TextBoxParameter2.Visible = true;
                    LabelParameter2.Visible = true;
                    LabelParameter2.Text = automation.ParameterDes[1];
                    PanelParameterOut.Visible = true;
                    TextBoxParameter3.Visible = false;
                    MailContentTypeCheckBoxList.Visible = true;
                    LabelParameter3.Text = automation.ParameterDes[2];
                    LabelParameter3.Visible = true;
                }
                else
                {
                    PanelParameterOut.Visible = true;
                    if (automation.ParameterDes[0].Equals(StringDef.ServerId))
                        ButtonServerIdSelect.Visible = true;
                    TextBoxParameter1.Visible = true;
                    LabelParameter1.Visible = true;
                    LabelParameter1.Text = automation.ParameterDes[0];

                    if (automation.ParameterDes[1].Equals(StringDef.ServerGroupId))
                        ButtonServerGroupIdSelect.Visible = true;
                    TextBoxParameter2.Visible = true;
                    LabelParameter2.Visible = true;
                    LabelParameter2.Text = automation.ParameterDes[1];
                    PanelParameterOut.Visible = true;
                    TextBoxParameter3.Visible = false;
                    LabelParameter3.Text = automation.ParameterDes[2];
                    LabelParameter3.Visible = false;
                }

                break;
        }
    }

    void LoadParameter(IAutomation automation)
    {
        switch ((int)automation.Parameter[0])
        {
            case 1:
                TextBoxParameter1.Text = automation.Parameter[1].ToString();
                break;
            case 2:
                TextBoxParameter1.Text = automation.Parameter[1].ToString();
                TextBoxParameter2.Text = automation.Parameter[2].ToString();
                break;
            case 3:
                TextBoxParameter1.Text = automation.Parameter[1].ToString();
                TextBoxParameter2.Text = automation.Parameter[2].ToString();

                //如果该任务的类型为“发邮件”
                if (automation.Guid == "{eca2ad62-0238-4996-82c0-852009463d94}")
                {                    
                    string contentTypes = automation.Parameter[3] as string;
                    foreach (ListItem item in MailContentTypeCheckBoxList.Items)
                    {
                        if (contentTypes.Contains(item.Value))
                        {
                            item.Selected = true;
                        }
                        else
                        {
                            item.Selected = false;
                        }
                    }
                }
                else
                {
                    TextBoxParameter3.Text = automation.Parameter[3].ToString();
                }


                break;
        }
    }

    void CreateCheckBoxListServer()
    {
        CheckBoxListServer.Items.Clear();
        foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
        {
            CheckBoxListServer.Items.Add(new ListItem(server.Name, server.Id.ToString()));
        }
    }

    void CreateCheckBoxListServerGroup()
    {
        CheckBoxListServerGroup.Items.Clear();
        foreach (ServerGroup group in AdminServer.TheInstance.GameServerManager.ServerGroups)
        {
            CheckBoxListServerGroup.Items.Add(new ListItem(group.Name, group.Id.ToString()));
        }
    }

    void CreateCheckBoxListFtpServer()
    {
        CheckBoxListFtpServer.Items.Clear();
        foreach (FTPServer ftpServer in AdminServer.TheInstance.FTPClient.FTPServerList)
        {
            if (ftpServer.FtpServerType == FTPServer.FTPServerType.ClientServerListFile)
                CheckBoxListFtpServer.Items.Add(new ListItem(ftpServer.Literal + "[" + ftpServer.Address + "]", ftpServer.ID.ToString()));
        }
    }

    void ResetServerListAndButton()
    {
        PanelServer.Visible = false;
        PanelFtpServer.Visible = false;
        PanelServerGroup.Visible = false;
        ButtonServerIdSelect.Text = StringDef.Select;
        ButtonFtpServerIdSelect.Text = StringDef.Select;
        ButtonServerGroupIdSelect.Text = StringDef.Select;
    }

    void LoadServerState(string stringIdList)
    {
        foreach (ListItem item in CheckBoxListServer.Items)
            item.Selected = false;

        if (stringIdList.Trim().Length != 0)
        {
            ArrayList notExist = new ArrayList();

            string[] ids = stringIdList.Split(' ');
            foreach (string id in ids)
            {
                if (id.Length != 0)
                {
                    bool exist = false;
                    foreach (ListItem item in CheckBoxListServer.Items)
                    {
                        if (item.Value.Equals(id, StringComparison.OrdinalIgnoreCase))
                        {
                            item.Selected = true;
                            exist = true;
                            break;
                        }
                    }
                    if (!exist) notExist.Add(id);
                }
            }
            if (notExist.Count != 0)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("ServerId:");
                foreach (string notExistId in notExist)
                {
                    sb.Append(notExistId);
                    sb.Append(',');
                }
                sb.Remove(sb.Length - 1, 1);
                sb.Append(" not exist.");
                LabelParameterInfo.Text = sb.ToString();
                LabelParameterInfo.Visible = true;
            }
        }
    }

    void LoadFtpServerState(string stringFtpIdList)
    {
        foreach (ListItem item in CheckBoxListFtpServer.Items)
            item.Selected = false;

        if (stringFtpIdList.Trim().Length != 0)
        {
            ArrayList notExist = new ArrayList();

            string[] ids = stringFtpIdList.Split(' ');
            foreach (string id in ids)
            {
                if (id.Length != 0)
                {
                    bool exist = false;
                    foreach (ListItem item in CheckBoxListFtpServer.Items)
                    {
                        if (item.Value.Equals(id, StringComparison.OrdinalIgnoreCase))
                        {
                            item.Selected = true;
                            exist = true;
                            break;
                        }
                    }
                    if (!exist) notExist.Add(id);
                }
            }
            if (notExist.Count != 0)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("ServerId:");
                foreach (string notExistId in notExist)
                {
                    sb.Append(notExistId);
                    sb.Append(',');
                }
                sb.Remove(sb.Length - 1, 1);
                sb.Append(" not exist.");
                LabelParameterInfo.Text = sb.ToString();
                LabelParameterInfo.Visible = true;
            }
        }
    }

    void LoadServerGroupState(string stringIdList)
    {
        foreach (ListItem item in CheckBoxListServerGroup.Items)
            item.Selected = false;

        if (stringIdList.Trim().Length != 0)
        {
            ArrayList notExist = new ArrayList();

            string[] ids = stringIdList.Split(' ');
            foreach (string id in ids)
            {
                if (id.Length != 0)
                {
                    bool exist = false;
                    foreach (ListItem item in CheckBoxListServerGroup.Items)
                    {
                        if (item.Value.Equals(id, StringComparison.OrdinalIgnoreCase))
                        {
                            item.Selected = true;
                            exist = true;
                            break;
                        }
                    }
                    if (!exist) notExist.Add(id);
                }
            }
            if(notExist.Count!=0)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append("ServerId:");
                foreach(string notExistId in notExist)
                {
                    sb.Append(notExistId);
                    sb.Append(',');                    
                }
                sb.Remove(sb.Length-1,1);
                sb.Append(" not exist.");
                LabelParameterInfo.Text = sb.ToString();
                LabelParameterInfo.Visible = true;
            }
        }        
    }
}
