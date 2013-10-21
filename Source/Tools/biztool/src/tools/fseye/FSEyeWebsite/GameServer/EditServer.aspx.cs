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
using System.IO;
using System.Xml;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;
using FSEye.DllWrap;

public partial class GameServer_EditServer : BasePage
{
	enum Operation
	{
		Add,
		Modify
	}

	GameServer _server;

	Operation _operation;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerEditServer, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		_operation = Operation.Add;
		string operation = Request.Params[WebConfig.ParamOperation];
		if (operation != null)
		{
			switch (operation)
			{
				case "add":
					_operation = Operation.Add;
					break;
				case "modify":
					_operation = Operation.Modify;
					break;
			}
		}

		if (_operation == Operation.Add)
		{
			TextBoxServerId.Text = StringDef.Unknown;
			LinkButtonEdit.Text = StringDef.Add;
            ButtonShowLordConfig.Enabled = false;
            ButtonShowServerConfig.Enabled = false;
		}
		else if(_operation == Operation.Modify)
		{
			LinkButtonEdit.Text = StringDef.Save;
			string serverIdText = Request.Params[WebConfig.ParamServerId];
			if (serverIdText != null)
			{
				int serverId = int.Parse(serverIdText);
				_server = TheAdminServer.GameServerManager.GetGameServer(serverId);
                if (_server != null)
                {
                    if (_server.LordConfig.FSEyeIp == null || _server.LordConfig.FSEyeIp.Length == 0)
                        LabelLordIsConfig.Visible = true;
                    else
                        LabelLordIsConfig.Visible = false;
                    //HyperLinkViewLordConfigFile.NavigateUrl = String.Format("GameServerHandler.ashx?op=CreateLordConfigFile&serverId={0}", serverId);
                    //HyperLinkViewLordConfigFile.NavigateUrl = String.Format("GameServerHandler.ashx?op=CreateGuardConfigFile&serverId={0}", serverId);
                    HyperLinkViewLordConfigFile.NavigateUrl = String.Format("ShowGuardConfig.aspx?serverId={0}", serverId);
                }
			}

			if (_server == null)
			{
				Response.Redirect("GameServer.aspx");
			}
			
			if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
			{
				Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
			}

			if (!IsPostBack)
			{
                FtpDropDownList.ListFtpByType(FTPServer.FTPServerType.ServerUpdate);
                
                ShowGameServerInfo();

                if (_operation == Operation.Modify)
                {
                    //备份服务器名称供提交时进行判断使用
                    ViewState["serverNameTemp"] = TextBoxServerName.Text;
                }
			}
            
            //my code//
            //当是Modify页面时禁止修改这两个DropDownList
            ListBoxServerType.Enabled = false;
            ServerGroupDropDownList.Enabled = false;
		}

		ServerGroupDropDownList.GroupType = ServerGroup.Type.Server;
        ServerGroupDropDownList.AutoPostBack = true;
        LabelOpMsg.Text = string.Empty;
        LabelSuccess.Visible = false;
        
        if (_operation == Operation.Add)
        {
            SetGameDir();
        }

        if (_operation == Operation.Add && !IsPostBack)
        {
            SetServerNameAndComment();
        }
        //给该Blaze控件增加EventHandler
        ServerGroupDropDownList.SelectedServerGroupChanged += new EventHandler(ServerGroupDropDownList_SelectedServerGroupChanged);
	}

    void ServerGroupDropDownList_SelectedServerGroupChanged(object sender, EventArgs e)
    {
        SetServerNameAndComment();
        //throw new Exception("The method or operation is not implemented.");
    }

    protected void ListBoxServerType_OnSelectedIndexChanged(object sender, EventArgs e)
    {
        SetServerNameAndComment();
    }

	private void ShowGameServerInfo()
	{
		if (_server != null)
		{
			TextBoxServerId.Text = _server.Id.ToString();
			TextBoxServerName.Text = _server.Name;
			TextBoxServerIpAddress.Text = _server.IpAddress;
			TextBoxServerComment.Text = _server.Comment;
			//TextBoxServerMacAddress.Text = _server.MacAddress;
			TextBoxGameDir.Text = _server.GameDir;
			//CheckBoxHasBeenConfigGuard.Checked = _server.ConfigState == GameServer.ConfigGuardState.Configed;
			ListBoxServerType.SelectedIndex = (int)_server.Type;
            ServerConfig.Text = _server.ConfigContent;
            ServerGroupDropDownList.SelectedServerGroup = _server.Group;
            FtpDropDownList.SelectFtp = _server.FtpServer;
		}
	}

    private void ShowLordConfig(LordConfig lordConfig)
    {
        int iServerCount = 0;
        string sServerIDs = "";
        if (lordConfig != null)
        {
            foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
            {
                if (server.IpAddress == TextBoxServerIpAddress.Text.Trim())
                {
                    if (iServerCount == 0)
                        sServerIDs = server.Id.ToString();
                    else
                        sServerIDs = sServerIDs + "," + server.Id.ToString();

                    iServerCount++;
                }
            }
            TextBoxLordPort.Text = iServerCount.ToString(); // lordConfig.ServerCount.ToString();
            TextBoxLordPort.Enabled = false;
            TextBoxMaxPlayer.Text = sServerIDs; // lordConfig.ServerIDs.ToString();
            TextBoxMaxPlayer.Enabled = false;

            TextBoxFSEyeIp.Text = lordConfig.FSEyeIp;
            TextBoxFSEyePort.Text = lordConfig.FSEyePort.ToString();

            if (_server.Type == GameServer.ServerType.gamecenter)
            {
                if (lordConfig.GMCIp != null)
                    GMCIpTextBox.Text = lordConfig.GMCIp;
                if (lordConfig.GMCAccount != null)
                    GMCAccountTextBox.Text = lordConfig.GMCAccount;
                
                GMCIpTr.Visible = true;
                GMCAccountTr.Visible = true;
            }
        }
    }

    private string GetServerConfigFileName(int severtype)
    {
       
        string serverConfigFileName = "";
        switch (severtype)
        {
            case (UInt16)GameServer.ServerType.unidentified:
                serverConfigFileName = ServerType2Module.Unidentified;
                break;
            case (UInt16)GameServer.ServerType.goddess:
                serverConfigFileName = ServerType2Module.RoleServer;
                break;
            case (UInt16)GameServer.ServerType.paysys:
                serverConfigFileName = ServerType2Module.Paysys;
                break;
            case (UInt16)GameServer.ServerType.bishop:
                serverConfigFileName = ServerType2Module.Gateway;
                break;
            case (UInt16)GameServer.ServerType.gameserver:
                serverConfigFileName = ServerType2Module.GameServer;
                break;
            case (UInt16)GameServer.ServerType.gamecenter:
                serverConfigFileName = ServerType2Module.GameCenter;
                break;
            case (UInt16)GameServer.ServerType.databaseserver:
                serverConfigFileName = ServerType2Module.DatabaseServer;
                break;
            case (UInt16)GameServer.ServerType.logserver:
                serverConfigFileName = ServerType2Module.LogServer;
                break;
        }

        return serverConfigFileName;
    }

    private bool GuardCfg(ref LordConfig lordConfig)
    {
        lordConfig = new LordConfig();
        lordConfig.ServerCount = int.Parse(TextBoxLordPort.Text);
        lordConfig.ServerIDs = TextBoxMaxPlayer.Text;

        lordConfig.FSEyeIp = TextBoxFSEyeIp.Text;
        lordConfig.FSEyePort = int.Parse(TextBoxFSEyePort.Text);

        if (_server.Type == GameServer.ServerType.gamecenter)
        {
            if ((GMCIpTextBox.Text != null) && (GMCIpTextBox.Text.Length != 0))
                lordConfig.GMCIp = GMCIpTextBox.Text;
            if ((GMCAccountTextBox.Text != null) && (GMCAccountTextBox.Text.Length != 0))
                lordConfig.GMCAccount = GMCAccountTextBox.Text;
        }

        return true;
    }

    private bool Save2DB()
    {
        try
        {
            //权限检查
            if (_operation == Operation.Add)
            {
                if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
                {
                    LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                    return false;
                }
            }
            else if (_operation == Operation.Modify)
            {
                int id = int.Parse(TextBoxServerId.Text);
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(id);

                if (server != null && !WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
                {
                    LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                    return false;
                }
            }
            else
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return false;
            }

            string name = TextBoxServerName.Text;
            string comment = TextBoxServerComment.Text;
            string ip = TextBoxServerIpAddress.Text;
            string gameDir = TextBoxGameDir.Text;
            string cfgContent = ServerConfig.Text;
            

            ////游戏目录最后一个“/”必须有
            //if (gameDir != null && gameDir.Length > 0)
            //    if (!gameDir.EndsWith(GameServer.FilePathDelimiter) && !gameDir.EndsWith(GameServer.FilePathWinDelimiter))
            //    {
            //        if (gameDir.IndexOf(GameServer.FilePathWinDelimiter) >= 0)
            //        {
            //            gameDir = gameDir + GameServer.FilePathWinDelimiter;
            //        }
            //        else
            //        {
            //            gameDir = gameDir + GameServer.FilePathDelimiter;
            //        }
            //        TextBoxGameDir.Text = gameDir;
            //    }

            ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
            LordConfig lordConfig = null;
            //GameServer.ConfigGuardState configState = CheckBoxHasBeenConfigGuard.Checked ? GameServer.ConfigGuardState.Configed : GameServer.ConfigGuardState.NotConfig;
            GameServer.ServerType serverType = (GameServer.ServerType)int.Parse(ListBoxServerType.SelectedValue);

            if (PanelLordConfig.Visible)
            {
                if (GuardCfg(ref lordConfig) == false)
                    return false;
            }

            if (_operation == Operation.Add)
            {
                int severtype = ListBoxServerType.SelectedIndex;
                string serverConfigFile = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + GetServerConfigFileName(severtype);
                ServerConfig.getServerConfigTextBox(serverConfigFile);
                if (TheAdminServer.GameServerManager.AddGameServer(name, comment, ip, gameDir, group, GameServer.ConfigGuardState.None, ServerConfig.Text, serverType, FtpDropDownList.SelectFtp))
                {
                    LabelSuccess.Visible = true;
                    LabelSuccess.Text = StringDef.AddGameServer + StringDef.Colon + StringDef.Success;
                }
                else
                {
                    LabelOpMsg.Text = StringDef.AddGameServer + StringDef.Colon + StringDef.Failure;
                    return false;
                }
            }
            else if (_operation == Operation.Modify)
            {
                int id = int.Parse(TextBoxServerId.Text);
                GameServer server = TheAdminServer.GameServerManager.GetGameServer(id);


                if (TheAdminServer.GameServerManager.ModifyGameServer(server, name, comment, ip, gameDir, group, lordConfig, GameServer.ConfigGuardState.Configuring, ServerConfig.Text, serverType, FtpDropDownList.SelectFtp))
                {
                    LabelSuccess.Visible = true;
                    LabelSuccess.Text = StringDef.ModifyServer + StringDef.Colon + StringDef.Success;
                    if (_server.LordConfig.FSEyeIp == string.Empty || _server.LordConfig.FSEyePort == 0)
                        LabelLordIsConfig.Visible = true;
                    else
                        LabelLordIsConfig.Visible = false;
                }
                else
                {
                    LabelOpMsg.Text = StringDef.ModifyServer + StringDef.Colon + StringDef.Failure;
                    return false;
                }
            }
        }
        catch (Exception ex)
        {
            string operationText = string.Empty;
            if (_operation == Operation.Add)
            {
                operationText = StringDef.AddServer;
            }
            else if (_operation == Operation.Modify)
            {
                operationText = StringDef.ModifyServer;
            }

            LabelOpMsg.Text = operationText + StringDef.Colon + StringDef.Failure + StringDef.Colon + ex.Message;
        }

        return true;
    }


	protected void LinkButtonSave_Click(object sender, EventArgs e)
	{
        //my code
        //在向数据库提交之前先检测是否已存在服务器使用该名称


        if (_operation == Operation.Modify)
        {
            if (TextBoxServerName.Text != (string)ViewState["serverNameTemp"])
            {
                if (serverNameConflict(TextBoxServerName.Text))
                {
                    LabelOpMsg.Text = "已有服务器使用该名称，请修改";
                    return;
                }
                else
                {
                    LabelOpMsg.Text = string.Empty;
                }
            }
        }
        
        if (_operation == Operation.Add)
        {
            if (serverNameConflict(TextBoxServerName.Text))
            {
                LabelOpMsg.Text = "已有服务器使用该名称，请修改";
                return;
            }
            else
            {
                LabelOpMsg.Text = string.Empty;
            }
        }

        Save2DB();

        if (_operation == Operation.Modify)
        {
            ViewState["serverNameTemp"] = TextBoxServerName.Text;
        }
	}

	protected void ButtonShowLordConfig_Click(object sender, EventArgs e)
	{
        if(_server == null)
        {
            //
            return;
        }
		if (PanelLordConfig.Visible)
		{
			PanelLordConfig.Visible = false;
			ButtonShowLordConfig.Text = StringDef.ShowLordConfig;
		}
		else
		{
            ShowLordConfig(_server.LordConfig);
            PanelLordConfig.Visible = true;
			PanelLordConfig.Focus();
			ButtonShowLordConfig.Text = StringDef.HideLordConfig;
		}
	}

    protected void ButtonShowServerConfig_Click(object sender, EventArgs e)
    {
        PanelServerConfig.Visible = !PanelServerConfig.Visible;
        if (PanelServerConfig.Visible == true)
        {
            ButtonShowServerConfig.Text = StringDef.Hide  + StringDef.ShowServerConfig;
        }
        else
        {
            ButtonShowServerConfig.Text = StringDef.Show + StringDef.ShowServerConfig;
        }        
    }	

    

    protected void LinkButtonAutoDeployLordConfig_Click(object sender, EventArgs e)
    {
        if (_server != null)
        {
            if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            else
            {
                try
                {
                    //选择服务器
                    SelectServerAutomation selectServer = new SelectServerAutomation();
                    selectServer.Parameter = new object[] { 2, _server.Id };
                    //创建Lord配置文件
                    CreateLordConfigAutomation createLord = new CreateLordConfigAutomation();
                    createLord.Parameter = new object[] { 1, _server.Id };
                    //上传文件
                    FileTransferAutomation fileTransAuto = new FileTransferAutomation();
                    string fileName = _server.Id + "_" + _server.Name + "_guard.cfg";
                    fileTransAuto.Parameter = new object[] { 1, _server.Id,fileName };
                    //配置Lord
                    ExeSysCmdAutomation exeSysAuto = new ExeSysCmdAutomation();
                    string scriptText = ShellScriptManager.TheInstance.GetScript("DeployLordConfig", _server.SystemName);
                    string file = _server.GuardDir + GameServer.FilePathDelimiter + GameServer.UploadFileDir + GameServer.FilePathDelimiter + fileName;
                    string comText = string.Format(scriptText, file, _server.GameDir);
                    exeSysAuto.Parameter = new object[] { 1, comText };
                    //删除FSEye本地存储文件
                    DeleteLocalFileAutomation deleteFile = new DeleteLocalFileAutomation();
                    deleteFile.Parameter = new object[] { 1, fileName };
                    
                    //把5个Automation连接起来
                    selectServer.Success = createLord;
                    createLord.Success = fileTransAuto;
                    fileTransAuto.Success = exeSysAuto;
                    exeSysAuto.Success = deleteFile;

					TheAdminServer.AutomationManager.Do(selectServer, new AutomationContext());

                    //Session["Auto"] = createLord;
                    //LiteralSaveFile1.Text = StringDef.Doing;
                    //UploadLordConfig.Visible = true;

                    //ShowMessage(StringDef.CreateLordConfigFile + " \"" + fileName + "\" " + StringDef.Colon + StringDef.Success, MessageType.Success);
                }
                catch (Exception)
                {
                }
            }
        }
    }

    /// <summary>
    /// 更新自动化配置的状态
    /// </summary>
    /// <param name="sender"></param>
    /// <param name="e"></param>
    //protected void uploadconfigstate(object sender, eventargs e)
    //{
    //    createlordconfigautomation createlord = null;
    //    if (session["auto"] != null) createlord = session["createlord"] as createlordconfigautomation;
    //    if (createlord == null)
    //    {
    //        //uploadlordconfig.visible = false;
    //        timer1.enabled = false;
    //        return;
    //    }
    //    if (createlord.state == automationstate.done)
    //    {
    //        literalsavefile1.text = createlord.result.tostring();
    //        if(createlord.result == automationresult.success)
    //            _server.appendmessage(stringdef.createlordconfigfile,gameserver.messageinfo.type.alert);
    //        if (createlord.result == automationresult.failure)
    //        {
    //            literaluploadfile1.text = automationresult.failure.tostring();
    //            literalconfig1.text = automationresult.failure.tostring();
    //            literaldeletefile1.text = automationresult.failure.tostring();
    //            session.remove("createlord");
    //        }
    //        else if (createlord.success.state == automationstate.done)
    //        {
    //            if(createlord.success.result == automationresult.success)
    //                _server.appendmessage(stringdef.uploadlordconfigfile,gameserver.messageinfo.type.alert);
    //            literaluploadfile1.text = createlord.success.result.tostring();
    //            if (createlord.success.result == automationresult.failure)
    //            {
    //                literalconfig1.text = automationresult.failure.tostring();
    //                literaldeletefile1.text = automationresult.failure.tostring();
    //                session.remove("createlord");
    //            }
    //            else if(createlord.success.success.state == automationstate.done)
    //            {
    //                if(createlord.success.success.result == automationresult.success)
    //                    _server.appendmessage(stringdef.configlord,gameserver.messageinfo.type.alert);
    //                literalconfig1.text = createlord.success.success.result.tostring();
    //                if (createlord.success.success.result == automationresult.failure)
    //                {
    //                    literaldeletefile1.text = automationresult.failure.tostring();
    //                    session.remove("createlord");
    //                }
    //                else if(createlord.success.success.success.state == automationstate.done)
    //                {
    //                    if(createlord.success.success.success.result == automationresult.success)
    //                    _server.appendmessage(stringdef.deletelocallordconfigfile,gameserver.messageinfo.type.alert);
    //                    literaldeletefile1.text = createlord.success.success.success.result.tostring();
    //                }
    //            }
    //        }
    //    }
    //}
    


    protected void SaveAsDefaultLordConfig_Click(object sender, EventArgs e)
    {
        try
        {
            LordConfig lordConfig = new LordConfig();
            lordConfig = new LordConfig();

            lordConfig.ServerCount = int.Parse(TextBoxLordPort.Text);
            lordConfig.ServerIDs = TextBoxMaxPlayer.Text;
            
           
            lordConfig.FSEyeIp = TextBoxFSEyeIp.Text;
            lordConfig.FSEyePort = int.Parse(TextBoxFSEyePort.Text);

            if (!LordConfig.SaveAsDefaultConfig(lordConfig))
            {
                LabelOpMsg.Text = StringDef.OperationFail;
                return;
            }
            else
            {
                LabelSuccess.Visible = true;
                LabelSuccess.Text = StringDef.OperationSucceed;
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
        }
    }

    protected void SetToDefaultLordConfig_Click(object sender, EventArgs e)
    {
        ShowLordConfig(LordConfig.LoadFromDefaultConfig());
    }

	protected void LinkButtonReconfigGuard_Click(object sender, EventArgs e)
	{
        if (Save2DB() == false)
            return;

        //发送GUARD配置文件
		if (_server != null)
		{
            if (_server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyReconfigureGuard))
            {
                LabelSuccess.Text += "<br />" + StringDef.Send + StringDef.Colon + StringDef.Success;
            }
            else
            {
                LabelOpMsg.Text += "<br />" + StringDef.Send + StringDef.Colon + StringDef.Failure;
            }
		}
	}

    protected void ButtonSendFile_Click(object sender, EventArgs e)
    {
        if (Save2DB() == false)
            return;

        //发送游戏服务器程序的配置
        if (_server.ConfigContent.Trim() == "")
        {
            LabelOpMsg.Text =　ButtonSendFile.Text + StringDef.Failure+StringDef.Colon+string.Format(StringDef.MsgCannotBeNone,StringDef.File);
        }
        else
        {
            SendFile(_server.ConfigContent, _server.GameDir + GetServerConfigFileName((int)_server.Type));
            //发送shm.ini
            //SendFile("[Server]" + SystemConfig.Current.DefaultNewLine + "Server=" + _server.Id.ToString() + SystemConfig.Current.DefaultNewLine, _server.GameDir + "shm.ini");
        }
    }


    private void SendFile(string Content,string RemoteFile)
    {        
        string[] para = new string[3];
        para[0] = _server.Id.ToString();
        para[1] = Content;
        para[2] = RemoteFile;

        if (_server.DoPlugInAction(CurrentUser.Id, E2gSendfile.PlugInGuid, E2gSendfile.actionKey, para))
        {
            LabelSuccess.Text += "<br />" + StringDef.Send + RemoteFile + StringDef.Colon + StringDef.Success + "<br />";
            LabelSuccess.Visible = true;
        }
        else
        {
            LabelOpMsg.Text += "<br />" + StringDef.Send + StringDef.Colon + StringDef.Failure;
        }
        para = null;
    }

    //my code
    //修改了此函数生成游戏目录的算法，解决了目录名冲突和多位数字的问题
    private void SetGameDir()
    {
        string GameDir = "../";
        int i=1;
        bool atLeaseOne = false;
        string maxNum;      //存储文件夹编号的最大值        
        string tempbit;     //文件夹编号的数字多于1位时，存储多读的那1位
        GameDir += Enum.GetName(typeof(GameServer.ServerType), int.Parse(ListBoxServerType.SelectedValue));
        ServerGroup svrGroup = ServerGroupDropDownList.SelectedServerGroup;
        foreach (GameServer childServer in svrGroup.List)
        {
            int j = childServer.GameDir.Length - 2;
            if (childServer.Id.ToString() != TextBoxServerId.Text)
            {
                if (((int)childServer.Type).ToString() == ListBoxServerType.SelectedValue)
                {
                    atLeaseOne = true;
                    maxNum = childServer.GameDir.Substring(j, 1);
                    if (Char.IsDigit(maxNum, 0))
                    {

                        //往前读1位，直到读到非数字为止
                        while (Char.IsDigit(childServer.GameDir.Substring(--j, 1), 0))
                        {
                            tempbit = childServer.GameDir.Substring(j, 1);
                            maxNum = tempbit + maxNum;
                        }

                        if (i < int.Parse(maxNum))
                        {
                            i = int.Parse(maxNum);
                        }
                    }                        
                }
            }
        }
        if (atLeaseOne == true)
        {
            i++;
            string iString = i.ToString();
            if (ListBoxServerType.SelectedItem.Text == StringDef.ServerTypeGameServer)
            {
                if (iString.Length == 1)
                {
                    iString = "0" + iString;
                }
            }
            GameDir += iString;
        }
        else
        {
            if (ListBoxServerType.SelectedItem.Text == StringDef.ServerTypeGameServer)
            {
                GameDir += "01";
            }
        }

        GameDir += "/";
        TextBoxGameDir.Text = GameDir;
        num = i;
    }

    //my code
    
    //修改name和comment两个textbox的内容
    private int num;    //该变量存储文件夹的编号，用来使自动生成的命名、备注与文件夹一致
    private void SetServerNameAndComment()
    {
        string tempString = ServerGroupDropDownList.SelectedServerGroup.Name + "_" + ListBoxServerType.SelectedItem.Text;

        if (ListBoxServerType.SelectedItem.Text == StringDef.ServerTypeGameServer)
        {
            if (num < 10)
                tempString += "0" + num.ToString();
            else
                tempString += num.ToString();
        }
        else
        {
            if (num >= 2)
                tempString += num.ToString();
        }

        TextBoxServerName.Text = tempString;
        TextBoxServerComment.Text = tempString;
    }

    //检查服务器名称是否存在冲突，调用了recursiveServerNameConflict(ServerGroup targetGroup, string serverName)
    private bool serverNameConflict(string serverName)
    {
        IList netGroupList = AdminServer.TheInstance.GameServerManager.TopServerGroups;
        foreach (ServerGroup netGroup in netGroupList)
        {
            if (recursiveServerNameConflict(netGroup, serverName))
            {
                return true;
            }
        }                        
        return false;
    }

    //递归检查服务器名称
    private bool recursiveServerNameConflict(ServerGroup targetGroup, string serverName)
    {
        if (targetGroup.GroupType == ServerGroup.Type.Group)
        {
            IList childGroupList = targetGroup.List;
            foreach (ServerGroup childGroup in childGroupList)
            {
                if (childGroup.GroupType == ServerGroup.Type.Server)
                {
                    foreach (GameServer server in childGroup.List)
                    {
                        if (server.Name == serverName)
                        {
                            return true;
                        }
                    }
                }
                else if (childGroup.GroupType == ServerGroup.Type.Group)
                {
                    if ((recursiveServerNameConflict(childGroup, serverName)))
                    {
                        return true;
                    }
                }
            }
        }
        else if (targetGroup.GroupType == ServerGroup.Type.Server)
        {
            foreach (GameServer server in targetGroup.List)
            {
                if (server.Name == serverName)
                {
                    return true;
                }
            }
        }

        return false;
    }
}