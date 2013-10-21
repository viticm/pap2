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
using System.Collections.Generic;
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.Cryptography;
using FSEye.PlugIn;

public partial class GameServer_ControlGame : BasePage
{
    private int _gameServerId;
    private string _operation;
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
        
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerServerControl, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
       
		if (!IsPostBack)
		{
			CreateOperationList();
		}
       //判断是启动还是关闭服务器
        if (Request.Params["serverId"] != null && Request.Params["operation"] != null)
        {
            _operation = Request.Params["operation"];
            if (_operation == "start")
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
                    GameServerCheckBoxList.SetServerSelected(_gameServerId, true);
                    GameServerCheckBoxList.ShowsServerSelected();
                    ListBoxOperation.SelectedIndex = 0;
                    ShowSelectedOperationPanel(0);
                }
            }
            else if (_operation == "stop")
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
                    GameServerCheckBoxList.SetServerSelected(_gameServerId, true);
                    GameServerCheckBoxList.ShowsServerSelected();
                    ListBoxOperation.SelectedIndex = 1;
                    ShowSelectedOperationPanel(1);
                }

            }
        }
            else
            {
                ShowSelectedOperationPanel();
            }

        

        AddTipToCheckBoxListPrivilege();

		ClearMessage();
        LabelOpMsg.Text = string.Empty;
	}

    private void AddTipToCheckBoxListPrivilege()
    {
        string[] Tips = new string[]{
            "Sets all simple privileges except GRANT OPTION",
            "Enables use of ALTER TABLE",
            "Enables stored routines to be altered or dropped",
            "Enables use of CREATE TABLE",
            "Enables creation of stored routines",
            "Enables use of CREATE TEMPORARY TABLE",
            "Enables use of CREATE USER, DROP USER, RENAME USER, and REVOKE ALL PRIVILEGES.",
            "Enables use of CREATE VIEW",
            "Enables use of DELETE",
            "Enables use of DROP TABLE",
            "Enables the user to run stored routines",
            "Enables use of SELECT ... INTO OUTFILE and LOAD DATA INFILE",
            "Enables use of CREATE INDEX and DROP INDEX",
            "Enables use of INSERT",
            "Enables use of LOCK TABLES on tables for which you have the SELECT privilege",
            "Enables the user to see all processes with SHOW PROCESSLIST",
            "Enables use of FLUSH",
            "Enables the user to ask where slave or master servers are",
            "Needed for replication slaves (to read binary log events from the master)",
            "Enables use of SELECT",
            "SHOW DATABASES shows all databases",
            "Enables use of SHOW CREATE VIEW",
            "Enables use of mysqladmin shutdown",
            "Enables use of CHANGE MASTER, KILL, PURGE MASTER LOGS, and SET GLOBAL statements, the mysqladmin debug command; allows you to connect (once) even if max_connections is reached",
            "Enables use of UPDATE",
            "Synonym for “no privileges”",
            "Enables privileges to be granted"
        };

        for(int checkIndex = 0;checkIndex<CheckBoxListPrivilege.Items.Count;++checkIndex)
        {
            ListItem item = CheckBoxListPrivilege.Items[checkIndex];
            //如果说明文字太长就使用WIDTH属性控制
            string width = string.Empty;
            if (Tips[checkIndex].Length >= 70) width = ",WIDTH,400";
            item.Attributes.Add("onmouseover", string.Format("Tip('{0}'{1});", Tips[checkIndex],width));
        }
    }

	private void CreateOperationList()
	{
		string[] OperationList = new string[] {
			StringDef.StartGame,
			StringDef.StopGame,
			StringDef.UpdateGame,
			StringDef.UpdateGuard,
            StringDef.SetServerTime,
			StringDef.DeleteServer,
			StringDef.CreateGameServerListFile,
            StringDef.AddGameDbUser,
            StringDef.DeleteGameDbUser,
			StringDef.BackupGameDb,
			StringDef.RollbackGameDb,
			StringDef.RestartGameDb,
			StringDef.RestartServer,
		};

		for (int i = 0; i < OperationList.Length; i++)
		{
			ListBoxOperation.Items.Add(OperationList[i]);
		}
		
		ListBoxOperation.SelectedIndex = 0;
	}

	private void ShowSelectedOperationPanel()
	{
		Panel[] OperationPanelList = new Panel[] {
			PanelStartGame,
			PanelStopGame,
			PanelUpdateGame,
			PanelUpdateGuard,
            PanelSetServerTime,
			PanelDelete,
			PanelCreateGameServerListFile,
            PanelAddDbUser,
            PanelDeleteDbUser,
			PanelBackupDb,
			PanelRollbackDb,
			PanelRestartDb,
			PanelRestartServer,
		};

		for (int i = 0; i < OperationPanelList.Length; i++)
		{
			OperationPanelList[i].Visible = false;
		}

		if (ListBoxOperation.SelectedIndex >= 0 && ListBoxOperation.SelectedIndex < OperationPanelList.Length)
		{
			OperationPanelList[ListBoxOperation.SelectedIndex].Visible = true;
		}
	}
    private void ShowSelectedOperationPanel(int panel)
	{
		Panel[] OperationPanelList = new Panel[] {
			PanelStartGame,
			PanelStopGame,
			PanelUpdateGame,
			PanelUpdateGuard,
            PanelSetServerTime,
			PanelDelete,
			PanelCreateGameServerListFile,
            PanelAddDbUser,
            PanelDeleteDbUser,
			PanelBackupDb,
			PanelRollbackDb,
			PanelRestartDb,
			PanelRestartServer,
		};

		for (int i = 0; i < OperationPanelList.Length; i++)
		{
			OperationPanelList[i].Visible = false;
		}

			OperationPanelList[panel].Visible = true;
		
	}

    protected void LinkButtonAddDbUser_Click(object sender, EventArgs e)
    {
        string userName = TextBoxAddDbUserUserName.Text.Trim();
        if (userName.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.UserName), null);
            return;
        }
        string password = TextBoxAddDbUserUserPassword.Text;
        string newUserName = TextBoxAddDbUserNewUserName.Text.Trim();
        if (newUserName.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.New + StringDef.UserName), null);
            return;
        }
        string newUserPassword = TextBoxAddDbUserNewUserPassword.Text;
        string host = TextBoxAddDbUserHost.Text;
        if (host.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.Host), null);
            return;
        }
        string databaseName = TextBoxAddDbUserDatabaseName.Text;
        if (databaseName.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.DatabaseName), null);
            return;
        }

        StringBuilder strPrivilege = new StringBuilder();
        if (CheckBoxListPrivilege.SelectedIndex == -1)
        {
            //没有权限
            strPrivilege.Append("USAGE");
        }
        else
        {
            if (CheckBoxListPrivilege.Items[0].Selected)
            {
                //All权限
                strPrivilege.Append("ALL");
            }
            else
            {
                foreach (ListItem item in CheckBoxListPrivilege.Items)
                {
                    if (item.Selected) strPrivilege.Append(item.Text + ',');
                }
                strPrivilege.Remove(strPrivilege.Length-1,1);
            }
        }

        string script = ShellScriptManager.TheInstance.GetScript(WebConfig.ScriptFileNameDBAddUsr,"", userName, password, databaseName, newUserName,
            newUserPassword, strPrivilege.Length == 0 ? string.Empty : strPrivilege.ToString(), host);

        try
        {
            GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
            int totalOperateServerCount = selectedGameServer.Length;
            int successOperateServerCount = 0;

            StringBuilder failMsgBuilder = new StringBuilder();
            ArrayList successList = new ArrayList();
            foreach (GameServer server in selectedGameServer)
            {
                if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyExeSysCmd, script, string.Empty, GameServerControl.GuardTask.DoNormalSysCmd))
                    {
                        successOperateServerCount++;
                        successList.Add(server);
                    }
                    else
                        failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                }
                else
                {
                    failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                }
            }

            ReportOperationResult(
                successOperateServerCount, 
                totalOperateServerCount, 
                StringDef.AddGameDbUser, successOperateServerCount == totalOperateServerCount ? string.Empty : failMsgBuilder.ToString()
            );

            if (successList.Count > 0)
            {
                StringBuilder opMsg = new StringBuilder();
                foreach (GameServer server in selectedGameServer)
                {
                    string result = server.GetPlugInData(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandOutput) as string;
                    for (int count = 0; count < 5; ++count)
                    {
                        if (result == null || result.Length == 0) break;
                        System.Threading.Thread.Sleep(1000);
                        result = server.GetPlugInData(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandOutput) as string;
                    }
                    opMsg.AppendFormat("[{0}]{1}<br />", server.Name, result);
                }
                if (opMsg.Length != 0) LabelOpMsg.Text = opMsg.ToString();
            }
        }
        catch (Exception ex)
        {
            ReportOperationError(ex.Message, StringDef.AddGameDbUser);
        }
    }

    protected void LinkButtonDeleteDbUser_Click(object sender, EventArgs e)
    {
        string userName = this.TextBoxDelDbUserUserName.Text.Trim();
        if (userName.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.UserName), null);
            return;
        }
        string UserPassword = TextBoxDelDbUserUserPassword.Text;

        string delUserName = TextBoxDelDbUserDelUserName.Text.Trim();
        if (delUserName.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.Current + StringDef.UserName), null);
            return;
        }
        
        string host = TextBoxDelDbUserHost.Text;
        if (host.Length == 0)
        {
            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.Host), null);
            return;
        }

        string script = ShellScriptManager.TheInstance.GetScript(WebConfig.ScriptFileNameDBDelUsr,"", userName, UserPassword, delUserName, host);
        try
        {
            GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
            int totalOperateServerCount = selectedGameServer.Length;
            int successOperateServerCount = 0;

            StringBuilder failMsgBuilder = new StringBuilder();
            ArrayList successList = new ArrayList();
            foreach (GameServer server in selectedGameServer)
            {
                if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyExeSysCmd, script, string.Empty, GameServerControl.GuardTask.DoNormalSysCmd))
                    {
                        successOperateServerCount++;
                        successList.Add(server);
                    }
                    else
                        failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                }
                else
                {
                    failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                }
            }

            ReportOperationResult(
                successOperateServerCount, 
                totalOperateServerCount, 
                StringDef.DeleteGameDbUser, 
                successOperateServerCount == totalOperateServerCount ? string.Empty : failMsgBuilder.ToString()
            );

            if (successList.Count > 0)
            {
                StringBuilder opMsg = new StringBuilder();
                foreach (GameServer server in selectedGameServer)
                {
                    string result = server.GetPlugInData(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandOutput) as string;
                    for (int count = 0; count < 5; ++count)
                    {
                        if (result == null || result.Length == 0) break;
                        System.Threading.Thread.Sleep(1000);
                        result = server.GetPlugInData(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.DataKeyLastSystemCommandOutput) as string;
                    }
                    opMsg.AppendFormat("[{0}]{1}<br />", server.Name, result);
                }
                if (opMsg.Length != 0) LabelOpMsg.Text = opMsg.ToString();
            }
        }
        catch (Exception ex)
        {
            ReportOperationError(ex.Message, StringDef.DeleteGameDbUser);
        }
    }

    protected void LinkButton_Click(object sender, EventArgs e)
       
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            try
            {
                GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
                int totalOperateServerCount = selectedGameServer.Length;
                int successOperateServerCount = 0;
                StringBuilder failMsgBuilder = new StringBuilder();
 
                switch (lb.CommandName)
                {
                    case "StartGame":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStartGame))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "StopGame":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStopGame))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "UpdateGame":
                        string patchName = TextBoxPatchName.Text.Trim();
                        if (patchName.Length == 0)
                        {
                            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.PatchName), null);
                            return;
                        }
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                object outArg = null;
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGame, ref outArg, patchName))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "UpdateGuard":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGuard))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "SetServerTime":
                        //不选择时间时,默认为FSEye当前时间
                        DateTime timeToSet = DateTime.MinValue;
                        if (DateTimeToSet.Selected)
                            timeToSet = DateTimeToSet.SelectedDate;
                        else
                            timeToSet = DateTime.Now;
                        string timeZoneFile = GameServerControl.GetLinuxZoneInfoFile();
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeySetServerTime, timeToSet, timeZoneFile))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "DeleteServer":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
                            {
                                if (TheAdminServer.GameServerManager.DeleteGameServer(server))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "CreateGameServerListFile":
                        ArrayList serversToList = new ArrayList();
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                serversToList.Add(server);
                                successOperateServerCount++;
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        if (serversToList.Count != 0)
                            TheAdminServer.GameServerManager.CreateServerListFiles((GameServer[])serversToList.ToArray(typeof(GameServer)));
                        break;
                    case "BackupDb":
                        if (RadioButtonListBackupMode.SelectedIndex == -1)
                        {
                            ReportOperationError(string.Format(StringDef.MsgCannotBeNone,StringDef.BackupMode),null);
                            return;
                        }
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyBackupDb, RadioButtonListBackupMode.SelectedIndex))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "RollbackDb":
                        string dumpFileName = TextBoxDumpFileName.Text;
                        if (dumpFileName.Length == 0)
                        {
                            ReportOperationError(string.Format(StringDef.MsgCannotBeNone, StringDef.FileName), null);
                            return;
                        }
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRollbackDb, dumpFileName))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "RestartDb":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRestartDb))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "RestartServer":
                        foreach (GameServer server in selectedGameServer)
                        {
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRestartServer))
                                    successOperateServerCount++;
                                else
                                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
                            }
                            else
                            {
                                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
                            }
                        }
                        break;
                    case "AddDbUser":
                        break;
                    case "DeleteDbUser":
                        break;
                }
                ReportOperationResult(successOperateServerCount, totalOperateServerCount, null, successOperateServerCount == totalOperateServerCount ? string.Empty : failMsgBuilder.ToString());
            }
            catch (Exception ex)
            {
                ReportOperationError(ex.Message, null);
            }
        }
    }

    void ReportOperationResult(int successCount, int totalCount, string operationDesc,string failMsg)
    {
        if (totalCount > 0)
        {
            //if (successCount == totalCount)
            //{
            //    LabelSuccess.Visible = true;
            //    LabelSuccess.Text = "发送命令成功！（成功" + StringDef.Colon + successCount + " / 总计" + StringDef.Colon + totalCount + "）";
            //}
            //else
            //{
            //    LabelFailure.Visible = true;
            //    LabelFailure.Text = "发送命令未能完全成功！（成功" + StringDef.Colon + successCount + " / 总计" + StringDef.Colon + totalCount + "）";
            //}

            LabelOpMsg.Text = StringDef.SendSuccess + StringDef.Colon + string.Format("{0}/{1}", successCount, totalCount);
            if (failMsg != null && failMsg.Length != 0)
            {
                LabelOpMsg.Text += "<br />";
                LabelOpMsg.Text += StringDef.SendFail + StringDef.Colon + StringDef.GameServer + failMsg.ToString();
            }
        }
        else
        {
            LabelFailure.Visible = true;
            LabelFailure.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);// "没有选择服务器！";
        }
    }

	void ReportOperationError(string errorMessage, string operationDesc)
	{
		LabelFailure.Visible = true;
		if (operationDesc != null && operationDesc.Length > 0)
			LabelFailure.Text = operationDesc + ": " + errorMessage;
		else
			LabelFailure.Text = errorMessage;
	}

    //protected void LinkButtonStartGame_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        StringBuilder failMsgBuilder = new StringBuilder();
    //        //failMsgBuilder.Append(StringDef.OperationFail + StringDef.Colon + StringDef.GameServer + ' ');
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.EXECUTE, Session))
    //            {
    //                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStartGame))
    //                {
    //                    successOperateServerCount++;
    //                }
    //                else
    //                {
    //                    failMsgBuilder.AppendFormat("[{0}] ", server.Name);
    //                }
    //            }
    //            else
    //            {
    //                failMsgBuilder.AppendFormat("[{0}][{1}] ", server.Name, StringDef.NotEnoughPrivilegeShort);
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null, failMsgBuilder.Length == 0 ? string.Empty : failMsgBuilder.ToString());
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonStopGame_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStopGame))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonUpdateGame_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        string patchName = TextBoxPatchName.Text;
    //        if (patchName != null && patchName.Length > 0)
    //        {
    //            GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //            int totalOperateServerCount = selectedGameServer.Length;
    //            int successOperateServerCount = 0;
    //            foreach (GameServer server in selectedGameServer)
    //            {					
    //                object outArg = null;
    //                if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGame, ref outArg, patchName))
    //                {
    //                    successOperateServerCount++;
    //                }
    //            }

    //            ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //        }
    //        else
    //        {
    //            ReportOperationError("没有输入更新包的名字！", null);
    //        }
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonUpdateGuard_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGuard))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonDelete_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (TheAdminServer.GameServerManager.DeleteGameServer(server))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonCreateGameServerListFile_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] servers = GameServerCheckBoxList.SelectedGameServers;
    //        TheAdminServer.GameServerManager.CreateServerListFiles(servers);
    //        LabelSuccess.Visible = true;

    //        LabelSuccess.Text = "创建游戏服务器列表文件：成功";
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonBackupDb_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        if (RadioButtonListBackupMode.SelectedIndex == -1)
    //        {
    //            ReportOperationError("未选择备份模式", null);
    //            return;
    //        }

    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {

    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyBackupDb, RadioButtonListBackupMode.SelectedIndex))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonRollbackDb_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            string dumpFileName = TextBoxDumpFileName.Text;
    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRollbackDb, dumpFileName))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonRestartDb_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRestartDb))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonRestartServer_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyRestartServer))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

    //protected void LinkButtonSetServerTime_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
    //        int totalOperateServerCount = selectedGameServer.Length;
    //        int successOperateServerCount = 0;
    //        foreach (GameServer server in selectedGameServer)
    //        {
    //            //不选择时间时,默认为FSEye当前时间
    //            DateTime timeToSet = DateTime.MinValue;
    //            if(DateTimeToSet.Selected)
    //                timeToSet = DateTimeToSet.SelectedDate;
    //            else
    //                timeToSet = DateTime.Now;

    //            string timeZoneFile = GameServerControl.GetLinuxZoneInfoFile();

    //            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeySetServerTime, timeToSet, timeZoneFile))
    //            {
    //                successOperateServerCount++;
    //            }
    //        }

    //        ReportOperationResult(successOperateServerCount, totalOperateServerCount, null);
    //    }
    //    catch (Exception ex)
    //    {
    //        ReportOperationError(ex.Message, null);
    //    }
    //}

	//protected void LinkButtonTurnOnPaysys_Click(object sender, EventArgs e)
	//{
	//    GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
	//    foreach (GameServer server in selectedGameServer)
	//    {
	//        server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeySetModule, 0, 0);
	//    }
	//}

	//protected void LinkButtonTurnOffPaysys_Click(object sender, EventArgs e)
	//{
	//    GameServer[] selectedGameServer = GameServerCheckBoxList.SelectedGameServers;
	//    foreach (GameServer server in selectedGameServer)
	//    {
	//        server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeySetModule, 0, -1);
	//    }
	//}

    //void ReportOperationResult(int successCount, int totalCount, string operationDesc)
    //{
    //    if (totalCount > 0)
    //    {
    //        if (successCount == totalCount)
    //        {
    //            LabelSuccess.Visible = true;
    //            LabelSuccess.Text = "发送命令成功！（成功" + StringDef.Colon + successCount + " / 总计" + StringDef.Colon + totalCount + "）";
    //        }
    //        else
    //        {
    //            LabelFailure.Visible = true;
    //            LabelFailure.Text = "发送命令未能完全成功！（成功" + StringDef.Colon + successCount + " / 总计" + StringDef.Colon + totalCount + "）";
    //        }
    //    }
    //    else
    //    {
    //        LabelFailure.Visible = true;
    //        LabelFailure.Text = "没有选择服务器！";
    //    }
    //}


}