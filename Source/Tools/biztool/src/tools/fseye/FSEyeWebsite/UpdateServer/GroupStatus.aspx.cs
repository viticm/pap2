using System;
using System.Data;
using System.Configuration;
using System.Collections;
using System.Collections.Generic;
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

public partial class GameServer_GroupStatus : BasePage
{
    static FileInfo[] fileInfos;
    static IList<object> gameServers = new List<object>();
    static ArrayList fileList = new ArrayList();
    UserAction jobType
    {
        get {
            if (Session["JobType"] == null)
                return UserAction.Nothing;
            else
                return (UserAction)Session["JobType"];
        }
                set { Session["JobType"] = value; }
    }

    string selectGroup
    {
        get
        {
            if (Session["SelectGroup"] == null)
                return "";
            else
                return Session["SelectGroup"].ToString();
        }
        set { Session["SelectGroup"] = value; }
    }

    DateTime timeStart
    {
        get {
            if (ViewState["TimeStart"] == null)
                ViewState["TimeStart"] = DateTime.Now;
            return (DateTime)ViewState["TimeStart"]; 
        }
        set { ViewState["TimeStart"] = value; }
    }

    Int64 SerialNum
    {
        get
        {
            if (ViewState["SerialNum"] == null)
            {
                return 0;
            }
            else
            {
                return Int64.Parse(ViewState["SerialNum"].ToString());
            }
        }
        set { ViewState["SerialNum"] = value; }
    }


    enum UserAction
    {
        Nothing,
        StartGame,
        StopGame,
        StopGS,
        Download,
        UpdateGame,
        UpdateGuard,
        OpenGroup,
        CloseGroup,
        SwitchToTree,
        Abort,
        SendConfigFile //发送所有配置文件,包括guard和游戏服务器
    }

    /// <summary>
    /// GoupID : 组ID
    /// Result : 最后结果
    /// GroupServers : 组里的本次执行的server的id, 用","分隔
    /// Success : 1为成功，0为失败
    /// </summary>
    static IList<string[]> ActionResultList = new List<string[]>();

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerGroupStatus, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }    
        if (Session["JobType"] != null)
        {
            Timer_GroupStatus.Enabled = true;
        }
        if (!Page.IsPostBack)
        {
            if (Session["TreeSelectGroup"] != null)
            {
                textboxServerGroupId.Text = Session["TreeSelectGroup"].ToString();
                Session["TreeSelectGroup"] = null;
            }
            LinkButtonExecute.Attributes.Add("onclick", "return confirm('是否执行操作？')");

            CreateOperationList();
        }
    }
    

    private bool InitAction(GameServer.ServerType serverType)
    {
        Session["bCheckboxEnable"] = false;
        ActionResultList.Clear();
        LabelOpMsg.Text = "";        
        lblStartTime.Text = StringDef.StartTime + StringDef.Colon + DateTime.Now;
        lblCostTime.Text = "";
        timeStart = DateTime.Now;
        //下载包
        if (fileList != null) fileList.Clear();
        if (fileInfos != null && CheckBoxListUploadedFile != null && CheckBoxListUploadedFile.Items.Count > 0)
        {
            for (int index = 0; index != fileInfos.Length; ++index)
            {
                ListItem item = CheckBoxListUploadedFile.Items[index] as ListItem;
                if (item.Selected)
                {
                    fileList.Add(fileInfos[index].Name);
                    
                    lblStartTime.Text += "  " + item.Text;

                    //MD5
                    if (checkboxMD5.Checked)
                    {
                        try
                        {
                            FileStream get_file = new FileStream(fileInfos[index].FullName, FileMode.Open, FileAccess.Read, FileShare.Read);
                            byte[] pdfFile = new byte[get_file.Length];
                            get_file.Read(pdfFile, 0, (int)get_file.Length);//将文件流读取到Buffer中
                            get_file.Close();
                            string strMD5 = MD5Buffer(pdfFile, 0, pdfFile.Length);
                            fileList.Add(strMD5);
                            lblStartTime.Text += "(" + strMD5 + ")";//对Buffer中的字节内容算MD5
                        }
                        catch (Exception ex)
                        {
                            LabelOpMsg.Text = ex.Message;
                            Util.DebugLog(ex.Message);
                        }
                    }
                }
            }
        }

        selectGroup = textboxServerGroupId.Text;

        if (gameServers != null) gameServers.Clear();

        GetServerOrder(serverType);

        if (selectGroup == null || selectGroup == string.Empty)
        {
            LabelOpMsg.Text = StringDef.Failure + StringDef.Colon +  StringDef.SelectNoGameServer;
            return false;
        }
        else if (jobType == UserAction.Download || jobType == UserAction.UpdateGame )
        {
            if (fileList == null || fileList.Count == 0)
            {
                LabelOpMsg.Text = StringDef.Failure + StringDef.Colon + StringDef.NoFiles;
                return false;
            }
            else
            {
                Timer_GroupStatus.Enabled = true;
            }
        }
        else
        {
            Timer_GroupStatus.Enabled = true;
        }

        return true;
    }

    private void SetFileList(bool bEnable)
    {
        foreach (ListItem chk in CheckBoxListUploadedFile.Items)
        {
            chk.Enabled = bEnable;
        }
    }


    //更新GUARD 
   protected void UpdateGuard()
   {
       jobType = UserAction.UpdateGuard;
       if (InitAction(GameServer.ServerType.unidentified))
       {
           SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.UpdateGuard, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
           LabelSerialNum.Text = SerialNum.ToString();
           for (int j = 0; j < gameServers.Count; j++)
           {
               IList servers = (IList)gameServers[j];

               for (int i = 0; i < servers.Count; i++)
               {
                   GameServer server = servers[i] as GameServer;
                   if (server != null)
                   {
                       if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                       {
                           if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyUpdateGuard))
                           {
                               WriteLog(server.Id, GameServer.ServerOperationType.UpdateGuard, true, "【" + server.IpAddress + "】" + StringDef.Success);
                           }
                           else
                           {
                               WriteLog(server.Id, GameServer.ServerOperationType.UpdateGuard, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                           }
                       }
                       else
                       {
                           WriteLog(server.Id, GameServer.ServerOperationType.UpdateGuard, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                       }
                       servers[servers.IndexOf(server)] = null;
                   }
               }
           }
       }
    }

       //下载
    protected void buttonDownload()
    {
        if (CheckBoxShowFileList.Checked == false || CheckBoxListUploadedFile == null)
        {
            LabelOpMsg.Text = StringDef.Failure + StringDef.Colon + StringDef.NoFiles;
            return;
        }
        jobType = UserAction.Download;
        if (InitAction(GameServer.ServerType.unidentified))
        {
            SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.Download, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
            LabelSerialNum.Text = SerialNum.ToString();
        }
    }

    //启动
    protected void buttonStart()
    {
        jobType = UserAction.StartGame;
        if (InitAction(GameServer.ServerType.unidentified))
        {
            SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.Start, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
            LabelSerialNum.Text = SerialNum.ToString();
        }
    }

     //关闭
    protected void buttonStop(GameServer.ServerType serverType)
    {
        jobType = UserAction.StopGame;
        if (InitAction(serverType))
        {
            SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.Close, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
            LabelSerialNum.Text = SerialNum.ToString();
        }
    }

    //更新
    protected void buttonUpdateGame()
    {
        if (CheckBoxShowFileList.Checked == false || CheckBoxListUploadedFile == null)
        {
            LabelOpMsg.Text = StringDef.Failure + StringDef.Colon + StringDef.NoFiles;
            return;
        }
        jobType = UserAction.UpdateGame;
        if (InitAction(GameServer.ServerType.unidentified))
        {
            SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.Update, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
            LabelSerialNum.Text = SerialNum.ToString();
        }
    }


    private void AbortAction()
    {
        SetUnDownloadState();
        textboxServerGroupId.Text = "";
        selectGroup = "";
        Timer_GroupStatus.Enabled = false;
        ShowRuntime();
        Session["JobType"] = null;
        Session["bCheckboxEnable"] = null;
        LabelOpMsg.Text = StringDef.Abort;
    }


    //清除downloading状态
    private void SetUnDownloadState()
    {
        if (selectGroup == null)
        {
            selectGroup = textboxServerGroupId.Text;
        }
        string[] groupid = selectGroup.Split(';');
        foreach (string gid in groupid)
        {
            if (gid == string.Empty) continue;
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));

            if (group.GroupType == ServerGroup.Type.Server)
            {
                foreach (GameServer server in group.List)
                {
                    //if (server.UpdateOrDownState == GameServer.UpdateState.Downloading | )
                        server.UpdateOrDownState = GameServer.UpdateState.Nothing;                    
                }
            }
        }
    }
    
    protected void Timer_GroupStatus_Tick1(object sender, EventArgs e)
    {
        try
        {
            if (Session["JobType"] == null || SerialNum == 0)
            {
                Timer_GroupStatus.Enabled = false;
                return;
            }

            TimeSpan timeCost = DateTime.Now - timeStart;
            lblCostTime.Text = StringDef.CostTime + StringDef.Colon + timeCost.Hours.ToString() + StringDef.Hour + timeCost.Minutes.ToString() + StringDef.Minute + timeCost.Seconds.ToString() + StringDef.Second;

                lock (this)
                {

                    //执行完毕
                    if (gameServers.Count == 0 || selectGroup == null || selectGroup == string.Empty )
                    {                        
                        AbortAction();
                        Session["ActionResultList"] = ActionResultList;

                        //如果是启动或关闭服务器组，则需要生成并上传serverlist文件
                        try
                        {
                            if (jobType == UserAction.StartGame || jobType == UserAction.StopGame)
                            {
                                CreateServerListFilesAutomation automationCreate = new CreateServerListFilesAutomation();
                                UploadServerListFilesToFtpAutomation automationUpload = new UploadServerListFilesToFtpAutomation();
                                automationCreate.Success = automationUpload;
                                automationUpload.FtpServerIdList = new ArrayList();
                                automationUpload.TimeOutSeconds = 10;
                                foreach (FTPServer ftpServer in AdminServer.TheInstance.FTPClient.FTPServerList)
                                {
                                    if (ftpServer.FtpServerType == FTPServer.FTPServerType.ClientServerListFile)
                                        automationUpload.FtpServerIdList.Add(ftpServer.ID);
                                }

                                TheAdminServer.AutomationManager.Do(automationCreate);
                            }
                        }
                        catch (Exception ex)
                        {
                            Util.DebugLog(ex.ToString());
                        }

                        Page.Response.Redirect("CurrentRunResult.aspx?SerialNum=" + SerialNum.ToString());
                    }

                    //删除为null的数据
                    for (int j = gameServers.Count - 1; j >= 0; j--)
                    {
                        IList servers = (IList)gameServers[j];

                        for (int i = servers.Count - 1; i >= 0; i--)
                        {
                            if (servers[i] == null) servers.RemoveAt(i);
                        }
                        if (servers.Count == 0) gameServers.RemoveAt(j);
                    }
                }


            if (jobType == UserAction.Download)
            {
                    DownloadPackFile();
            }
            else if (jobType == UserAction.StartGame)
            {
                    StarServer();
            }
            else if (jobType == UserAction.StopGame)
            {
                    StopServer();
            }
            else if (jobType == UserAction.UpdateGame)
            {
                    UpdateServer();                    
            }

            ShowRuntime();
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;

            textboxServerGroupId.Text = "";
            selectGroup = "";
            Timer_GroupStatus.Enabled = false;
            //ActionResultList.Clear();
            ResultList.Rows.Clear();
            Session["JobType"] = null;
            Session["bCheckboxEnable"] = null;
            ViewState["SerialNum"] = null;
            ViewState["TimeStart"] = null;
        }
    }

    private void UpdateServer()
    {
        //同一IP只发送一次update命令
        for (int j = 0; j < gameServers.Count; j++)
        {
            IList servers = (IList)gameServers[j];

            for (int i = 0; i < servers.Count; i++)
            {
                GameServer server = servers[0] as GameServer;
                if (server != null)
                {
                    if (server.UpdateOrDownState != GameServer.UpdateState.Updating)
                    {
                        server.UpdateOrDownState = GameServer.UpdateState.Updating;
                        
                        if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                        {
                            string[] para = new string[3];
                            //暂时每次操作只允许选一个包
                            para[0] = fileList[0].ToString();
                            if (checkboxMD5.Checked)
                                para[1] = fileList[1].ToString();
                            else
                                para[1] = "";
                            para[2] = "0";
                            if (!server.DoPlugInAction(CurrentUser.Id, E2gUpdateserver.PlugInGuid, E2gUpdateserver.actionKey, para))
                            {
                                //发送更新命令失败
                                WriteLog(server.Id, GameServer.ServerOperationType.Update, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                                servers[servers.IndexOf(server)] = null;
                            }                    
                        }
                        else
                        {
                            WriteLog(server.Id, GameServer.ServerOperationType.Update,false, StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                            servers[servers.IndexOf(server)] = null;
                        }
                    }
                    else
                    {
                        if (server.UpdateMsg.Count > 0)
                        {   
                            string[] strMsg = server.UpdateMsg[server.UpdateMsg.Count - 1] as string[];

                            if (strMsg[0] == strMsg[1] || strMsg[2] == "0")
                            {
                                //所有步骤执行完毕或执行出错
                                servers[servers.IndexOf(server)] = null;
                                server.UpdateOrDownState = GameServer.UpdateState.Updated;
                                if (strMsg[2] == "0")
                                {
                                    WriteLog(server.Id, GameServer.ServerOperationType.Update, false, "【" + server.IpAddress + "】" + strMsg[3].ToString());
                                }
                                else
                                {
                                    SetResult(server.Id.ToString());
                                    WriteLog(server.Id, GameServer.ServerOperationType.Update, true, "【" + server.IpAddress + "】" + strMsg[3].ToString());
                                    server.Group.SetGroupVersion(strMsg[3].Substring(8));
                                    
                                    for (int k = 0; k < ActionResultList.Count; k++)
                                    {
                                        string[] strResult = ActionResultList[k] as string[];
                                        if (strResult[2].ToString().IndexOf("," + server.Id.ToString() + ",") >= 0)
                                        {
                                                strResult[1] = strResult[1].ToString()+ "  " +  strMsg[3].ToString();
                                                strResult[3] = "1";
                                                ActionResultList[k] = strResult;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

    }

    private void WriteLog(int serverID, GameServer.ServerOperationType opType,bool bSuccess, string ErrMsg)
    {
        for (int j = 0; j < ActionResultList.Count; j++)
        {
            string[] strResult = ActionResultList[j] as string[];
            if (strResult[2].ToString().IndexOf("," + serverID.ToString() + ",") >= 0)
            {
                if (bSuccess)
                {
                    SetResult(serverID.ToString());
                }
                else
                {
                    SetResult(serverID.ToString(), ErrMsg);                    
                }
                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(SerialNum, CurrentUser.Id, ServerGroup.Type.Group, int.Parse(strResult[0].ToString()), opType, bSuccess, ErrMsg, GetAllGroupAndFatherID());
            }
        }
    }

    private void DownloadPackFile()
    {
        //同一IP只发送一次download命令
        for (int j = 0; j < gameServers.Count; j++)
        {
            IList servers = (IList)gameServers[j];

            for (int i = 0; i < servers.Count; i++)
            {
                GameServer server = servers[0] as GameServer;
                
                    if (server != null)                    
                    {
                        if (server.UpdateOrDownState != GameServer.UpdateState.Downloading)
                        {
                            server.UpdateOrDownState = GameServer.UpdateState.Downloading;
                            
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (server.FtpServer != -1)
                                {
                                    string[] para = new string[4];
                                    FTPServer ftpSvr = AdminServer.TheInstance.FTPClient.GetFTPServer(server.FtpServer);
                                    if (ftpSvr == null)
                                    {
                                        //获取ftp失败
                                        WriteLog(server.Id, GameServer.ServerOperationType.Download, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.Unknown + StringDef.UpdateServer);
                                        servers[servers.IndexOf(server)] = null; 
                                    }
                                    else
                                    {
                                        para[0] = ftpSvr.Address;
                                        para[1] = ftpSvr.UserName;
                                        para[2] = ftpSvr.Password;
                                        para[3] = fileList[0].ToString();

                                        if (!server.DoPlugInAction(CurrentUser.Id, E2gDownload.PlugInGuid, E2gDownload.actionKey, para))                                        
                                        {
                                            //发送命令失败
                                            WriteLog(server.Id, GameServer.ServerOperationType.Download, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                                            servers[servers.IndexOf(server)] = null; 
                                        }
                                    }
                                }
                                else
                                {
                                    //获取ftp失败
                                    WriteLog(server.Id, GameServer.ServerOperationType.Download, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.Unknown + StringDef.UpdateServer);
                                    servers[servers.IndexOf(server)] = null; 
                               }
                            }
                            else
                            {
                                WriteLog(server.Id, GameServer.ServerOperationType.Download, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                                servers[servers.IndexOf(server)] = null; 
                            }
                        }
                        else
                        {
                            if (server.DownloadMsg == null) continue;
                            if (server.DownloadMsg[0] == "100" || server.DownloadMsg[1] == "0")
                            {
                                //server.GameServerState = GameServer.ServerState.Running;
                                servers[servers.IndexOf(server)] = null;
                                server.UpdateOrDownState = GameServer.UpdateState.Downloaded;
                                //所有步骤执行完毕或执行出错
                                if (server.DownloadMsg[1] == "0")
                                {
                                    WriteLog(server.Id, GameServer.ServerOperationType.Download, false, "【" + server.IpAddress + "】" + server.DownloadMsg[2].ToString());
                                }
                                else
                                {
                                    WriteLog(server.Id, GameServer.ServerOperationType.Download, true, "【" + server.IpAddress + "】" + server.DownloadMsg[2].ToString());
                                }
                            }
                        }
                    }
                }
            }
    }


    private void StopServer()
    {
        for (int j = 0; j < gameServers.Count; j++)
        {
            IList servers = (IList)gameServers[j];

            for (int i = 0; i < servers.Count; i++)
            {
                GameServer server = servers[i] as GameServer;
                if (server != null)
                {
                    if (server.GameServiceState == GameServer.ServiceState.Stopping)
                    {
                        if (server.PID != -1)
                         {
                             string[] para = new string[1] { server.Id.ToString() };
                             server.DoPlugInAction(SecurityManager.SystemAutomationId, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);
                          }
                     }
                    else if (server.GameServiceState == GameServer.ServiceState.ErrorStopped || server.GameServiceState == GameServer.ServiceState.OKStopped)
                    {
                        string[] para = new string[1] { server.Id.ToString() };
                        server.DoPlugInAction(SecurityManager.SystemAutomationId, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);

                        if (server.PID == -1 || server.PID == 0)
                        {
                            servers[servers.IndexOf(server)] = null;
                            WriteLog(server.Id, GameServer.ServerOperationType.Close, true, "【" + server.IpAddress + "】" + server.Name);
                            AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, true, "【" + server.IpAddress + "】" + server.Name, server.Id.ToString());
                        }
                    }
                    else if (server.GameServiceState != GameServer.ServiceState.Stopping)
                    {
                        if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                        {
                            if (!server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStopGame))
                            {
                                //发送关闭命令失败                                
                                WriteLog(server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                                servers[servers.IndexOf(server)] = null;
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage, server.Id.ToString());
                            }
                        }
                        else
                        {
                            WriteLog(server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                            servers[servers.IndexOf(server)] = null;
                            AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Close, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege, server.Id.ToString());
                        }

                    }
                    //可以通过GC关闭GS；当GC关闭命令成功发送时，把所有GC下面的GS置成Stopping。否则系统会判断成意外关闭
                    //这样的算法比较麻烦，还是按启动的逆序，先从gs关起
                    if (server.Type == GameServer.ServerType.gameserver)
                    {
                        int iNext = i +1;
                        if (iNext < servers.Count)
                        {
                            GameServer svr = servers[iNext] as GameServer;
                            if (svr != null && svr.Type == GameServer.ServerType.gameserver)
                                continue;                            
                        }
                    }
                    
                        break;
                    
                }
            }
        }
    }

    private void GetServerOrder(GameServer.ServerType serverType)
    {
        string ipAddr = ","; //记录本组所有GUARD的IP
        string[] groupid = selectGroup.Split(';');

        
        string[] strResult;
        string groupServers = ","; //记录本组所有执行操作的server的ip
        

        if (jobType == UserAction.Download) 
        {  
                foreach (string gid in groupid)
                {                    
                    if (gid == string.Empty) continue;
                    ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                    groupServers = ",";
                    if (group.GroupType == ServerGroup.Type.Server)
                    {
                        //执行队列
                        IList<GameServer> gameServerOrder = new List<GameServer>();
                        foreach (GameServer server in group.List)
                        {

                            if (server.GameServerState == GameServer.ServerState.Unknown)
                            {
                                LabelOpMsg.Text = group.Name +server.Name + StringDef.Colon + StringDef.NoConnectionAlert;
                                continue;
                            }

                            groupServers += server.Id.ToString() + ",";

                            server.DownloadMsg[0] = "0";
                            server.DownloadMsg[1] = "1";
                            server.DownloadMsg[1] = ",";
                            server.UpdateOrDownState = GameServer.UpdateState.Nothing;
                            if (ipAddr.IndexOf("," + server.IpAddress + ",") == -1)
                            {
                                ipAddr += server.IpAddress + ",";                                
                                gameServerOrder.Add(server);
                            }
                        }                        
                        gameServers.Add(gameServerOrder);

                        //结果队列
                        strResult = new string[4] { gid,lblStartTime.Text+  "download initialization", groupServers,"1" };
                        ActionResultList.Add(strResult);                       
                    }
                }
        }
        else if(jobType == UserAction.StartGame)
        {
                foreach (string gid in groupid)
                {
                    if (gid == string.Empty) continue;
                    ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                    groupServers = ",";
                    if (group.GroupType == ServerGroup.Type.Server)
                    {
                        IList<GameServer> gameServerOrder = new List<GameServer>();
                        foreach (GameServer.StartOrder startOrder in Enum.GetValues(typeof(GameServer.StartOrder)))
                        {
                            foreach (GameServer server in group.List)
                            {
                                if (server.GameServerState == GameServer.ServerState.Unknown)
                                {
                                    LabelOpMsg.Text = group.Name + server.Name + StringDef.Colon + StringDef.NoConnectionAlert;
                                    continue;
                                }

                                groupServers += server.Id.ToString() + ",";

                                if (server.Type.ToString().Equals(startOrder.ToString(), StringComparison.OrdinalIgnoreCase))
                                {
                                    gameServerOrder.Add(server);
                                }
                            }
                        }
                        gameServers.Add(gameServerOrder);
                        //结果队列
                        strResult = new string[4] { gid, lblStartTime.Text + "start initialization", groupServers, "1" };
                        ActionResultList.Add(strResult);
                    }
                }
        }
        else if(jobType == UserAction.StopGame)
        {
                foreach (string gid in groupid)
                {
                    if (gid == string.Empty) continue;
                    ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                    groupServers = ",";
                    if (group.GroupType == ServerGroup.Type.Server)
                    {
                        IList<GameServer> gameServerOrder = new List<GameServer>();
                        
                        foreach (GameServer.StopOrder stopOrder in Enum.GetValues(typeof(GameServer.StopOrder)))
                        {
                            foreach (GameServer server in group.List)
                            {
                                if (serverType == GameServer.ServerType.unidentified)
                                {
                                    if (server.GameServerState == GameServer.ServerState.Unknown)
                                    {
                                        LabelOpMsg.Text = group.Name + server.Name + StringDef.Colon + StringDef.NoConnectionAlert;
                                        continue;
                                    }

                                    groupServers += server.Id.ToString() + ",";

                                    if (server.Type.ToString().Equals(stopOrder.ToString(), StringComparison.OrdinalIgnoreCase))
                                    {
                                        gameServerOrder.Add(server);
                                    }
                                }
                                else if (server.Type == serverType)
                                {
                                    if (server.GameServerState == GameServer.ServerState.Unknown)
                                    {
                                        LabelOpMsg.Text = group.Name + server.Name + StringDef.Colon + StringDef.NoConnectionAlert;
                                        continue;
                                    }

                                    groupServers += server.Id.ToString() + ",";

                                    if (server.Type.ToString().Equals(stopOrder.ToString(), StringComparison.OrdinalIgnoreCase))
                                    {
                                        gameServerOrder.Add(server);
                                    }
                                }
                            }
                        }
                        gameServers.Add(gameServerOrder);
                        //结果队列
                        strResult = new string[4] { gid, lblStartTime.Text + "stop initialization", groupServers, "1" };
                        ActionResultList.Add(strResult);
                    }
                }
        }
        else if(jobType == UserAction.UpdateGame || jobType == UserAction.UpdateGuard || jobType == UserAction.SendConfigFile)
        {
                foreach (string gid in groupid)
                {
                    if (gid == string.Empty) continue;
                    ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                    groupServers = ",";
                    if (group.GroupType == ServerGroup.Type.Server)
                    {
                        IList<GameServer> gameServerOrder = new List<GameServer>();
                        foreach (GameServer server in group.List)
                        {
                            if (server.GameServerState == GameServer.ServerState.Unknown)
                            {
                                LabelOpMsg.Text = group.Name + server.Name + StringDef.Colon + StringDef.NoConnectionAlert;
                                continue;
                            }

                            groupServers += server.Id.ToString() + ",";
                            server.UpdateOrDownState = GameServer.UpdateState.Nothing;
                            server.UpdateMsg.Clear();
                            if (ipAddr.IndexOf(server.IpAddress) == -1)
                            {
                                ipAddr += "," + server.IpAddress;
                                gameServerOrder.Add(server);
                            }
                        }
                        gameServers.Add(gameServerOrder);
                        //结果队列
                        strResult = new string[4] { gid, lblStartTime.Text + "update initialization", groupServers, "1" };
                        ActionResultList.Add(strResult);
                    }
                }
        }        

        //获取PID
        for (int j = 0; j < gameServers.Count; j++)
        {
            IList servers = (IList)gameServers[j];

            for (int i = 0; i < servers.Count; i++)
            {
                GameServer server = servers[i] as GameServer;
                if (server != null)
                {
                        string[] para = new string[1] { server.Id.ToString() };
                        server.DoPlugInAction(SecurityManager.SystemAutomationId, E2gServerPID.PlugInGuid, E2gServerPID.actionKey, para);                 
                }
            }
        }
    }

    protected void StarServer()
    {     
        //按排序后的列表启动，每组服务器按类别启动
        //实际是使用二维数组来保存服务器

        for (int j = 0; j < gameServers.Count; j++)
        {
            IList servers = (IList)gameServers[j];

            for (int i = 0; i < servers.Count; i++)
            {
                GameServer server = servers[i] as GameServer;
                    if (server != null)
                    {
                        if (server.GameServiceState == GameServer.ServiceState.Running)
                        {
                            servers[servers.IndexOf(server)] = null;
                            WriteLog(server.Id, GameServer.ServerOperationType.Start, true, "【" + server.IpAddress + "】" + server.Name);
                            AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, true, "【" + server.IpAddress + "】" + server.Name, server.Id.ToString());
                        }
                        else if (server.GameServiceState != GameServer.ServiceState.Starting)
                        {
                         //   server.GameServiceState = GameServer.ServiceState.Starting;
                            if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                            {
                                if (!server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyStartGame))
                                {
                                    if (server.Type == GameServer.ServerType.gameserver)
                                    {
                                        servers[servers.IndexOf(server)] = null;
                                    }
                                    else
                                    {
                                        //如果不是gs启动失败，后面的其他类型服务器没有启动必要
                                        for (int k = servers.Count - 1; k >= 0; k--)
                                        {
                                            servers[k] = null;
                                        }
                                    }

                                    WriteLog(server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                                    AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.SendMessage,server.Id.ToString());
                                }                                
                            }
                            else
                            {
                                WriteLog(server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                                servers[servers.IndexOf(server)] = null;
                                AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Server, server.Id, GameServer.ServerOperationType.Start, false, "【" + server.IpAddress + "】" + server.Name + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege,server.Id.ToString());
                            }
                        }

                        //=================目前guard似乎不支持同时执行多个任务===================
                        break;
                        //目前一组服务器里面只有gameserver是存在多台的情况
                        //if (server.Type != GameServer.ServerType.gameserver) break;
                    }
                }
            }
    }

    //写入错误消息
    private void SetResult(string svrID,string strMsg)
    {
        svrID = "," + svrID + ",";
        for (int j = 0; j < ActionResultList.Count; j++)
        {
            string[] strResult = ActionResultList[j] as string[];            
            if (strResult[2].ToString().IndexOf(svrID) >= 0)
            {
                strResult[1] = strMsg;
                strResult[3] = "0";
                ActionResultList[j] = strResult;
                break;
            }
        }
    }

    //写入成功消息
    private void SetResult(string svrID)
    {
        svrID = "," + svrID + ",";
        for (int j = 0; j < ActionResultList.Count; j++)
        {
            string[] strResult = ActionResultList[j] as string[];
            if (strResult[2].ToString().IndexOf(svrID) >= 0)
            {
                if (strResult[1].ToString().IndexOf("initialization") >= 0)
                {
                    //特殊判断
                    strResult[1] = strResult[1].ToString().Replace("initialization", "ok");
                    strResult[3] = "1";
                    ActionResultList[j] = strResult;
                }
                break;
            }
        }
    }

    private void ListUploadedFiles()
    {
        CheckBoxListUploadedFile.Items.Clear();
        TheAdminServer.ListUploadedFiles(out fileInfos);
        long totleFileLength = 0;
        if (fileInfos != null && fileInfos.Length > 0)
        {
            LabelUploadedNoFile.Visible = false;
            foreach (FileInfo file in fileInfos)
            {
                totleFileLength += file.Length;
                CheckBoxListUploadedFile.Items.Add(new ListItem(file.Name, file.FullName));
            }
        }
        else
        {
            LabelUploadedNoFile.Visible = true;
        }
        string totleFileLengthText = string.Empty;
        if (totleFileLength < 1024)
        {
            totleFileLengthText = Decimal.Round((decimal)totleFileLength, 2).ToString() + "B";
        }
        else if (totleFileLength < 1024 * 1024)
        {
            totleFileLengthText = Decimal.Round((decimal)totleFileLength / (decimal)1024, 2).ToString() + "K";
        }
        else
        {
            totleFileLengthText = Decimal.Round(totleFileLength / (decimal)1024 / (decimal)1024, 2) + "M";
        }
        LabelUploadedFilesInfo.Text = String.Format(StringDef.UploadFileDirSummary, fileInfos.Length, totleFileLengthText);
    }

    protected void ButtonSelectAllFiles_Click(object sender, EventArgs e)
    {
        SetAllFiles(true);
    }

    protected void ButtonSelectNoFile_Click(object sender, EventArgs e)
    {
        SetAllFiles(false);
    }

    protected void CheckBoxShowFileList_Click(object sender, EventArgs e)
    {
        if (CheckBoxShowFileList.Checked) ListUploadedFiles();
        PanelSelectFiles.Visible = CheckBoxShowFileList.Checked;
    }

    private void SetAllFiles(bool select)
    {
        foreach (ListItem item in CheckBoxListUploadedFile.Items)
        {
            item.Selected = select;
        }
    }

    private string MD5Buffer(byte[] pdfFile, int index, int count)
    {
        System.Security.Cryptography.MD5CryptoServiceProvider get_md5 = new System.Security.Cryptography.MD5CryptoServiceProvider();
        byte[] hash_byte = get_md5.ComputeHash(pdfFile, index, count);

        string result = System.BitConverter.ToString(hash_byte);
        result = result.Replace("-", "");
        return result;
    }

    private string MD5String(string str)
    {
        byte[] MD5Source = System.Text.Encoding.ASCII.GetBytes(str);
        return MD5Buffer(MD5Source, 0, MD5Source.Length);

    }

    private void ShowRunResult()
    {
        foreach (string[] strResult in ActionResultList)
        {
            TableRow row = new TableRow();
            TableCell[] cell = new TableCell[2];
            for (int i = 0; i <= 1; i++) cell[i] = new TableCell();
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(strResult[0].ToString()));

            cell[0].Text = GetGroupFullName(group, group.Name);

            cell[1].Text = strResult[1];
            if (strResult[3].ToString() == "0")
            {
                cell[1].BackColor = System.Drawing.Color.Red;
            }
            row.Cells.AddRange(cell);
            ResultList.Rows.Add(row);

        }
    }

    private string GetGroupFullName(ServerGroup group, string groupPath)
    {
        FSEyeObject groupParent = group.SecurityObject.Parent;
        while (groupParent.Parent != null)
        {
            ServerGroup parentGroup = TheAdminServer.GameServerManager.GetGameServerGroup(groupParent.Id);
            if (parentGroup == null) break;
            groupPath = parentGroup.Name + "/" + groupPath;
            groupParent = parentGroup.SecurityObject.Parent;
        }

        return groupPath;
    }

    private string GetAllGroupFullName()
    {
        string allName = "";
        string[] groupid = selectGroup.Split(';');
        foreach (string gid in groupid)
        {
            if (gid == string.Empty) continue;
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
            allName += GetGroupFullName(group, group.Name);
            allName += ";";
        }
        return allName;
    }

    private string GetAllGroupAndFatherID()
    {
        string allID = selectGroup;
        string[] groupid = selectGroup.Split(';');
        foreach (string gid in groupid)
        {
            if (gid == string.Empty) continue;
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
            if (group != null)
            {
                FSEyeObject groupParent = group.SecurityObject.Parent;
                while (groupParent != null)
                {
                    allID += groupParent.Id + ";";

                    ServerGroup parentGroup = TheAdminServer.GameServerManager.GetGameServerGroup(groupParent.Id);
                    if (parentGroup == null) break;
                    
                    groupParent = parentGroup.SecurityObject.Parent;
                }
            }
        }
        return allID;
    }

    private string GetGroupAndFatherID(int gid)
    {
        string allID = gid.ToString();
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(gid);
            FSEyeObject groupParent = group.SecurityObject.Parent;
            while (groupParent.Parent != null)
            {
                allID += groupParent.Id + ";";
            }
        return allID;
    }

    private void ShowRuntime()
    {
        ResultList.Rows.Clear();
        TableHeaderRow header = new TableHeaderRow();
        TableHeaderCell[] head = new TableHeaderCell[2];
        for (int i = 0; i <= 1; i++) head[i] = new TableHeaderCell();
        head[0].Width = new Unit(10f, UnitType.Percentage);
        head[0].Text = StringDef.Name;
        head[1].Width = new Unit(30f, UnitType.Percentage);
        head[1].Text = StringDef.Message;

        header.Cells.AddRange(head);
        ResultList.Rows.Add(header);

            ShowRunResult();
    }

    
    private void CreateOperationList()
    {
        ListBoxOperation.Items.Add(new ListItem(StringDef.StartGame, UserAction.StartGame.ToString()));
        ListBoxOperation.Items.Add(new ListItem( StringDef.StopGame,UserAction.StopGame.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.OnlyStopGS, UserAction.StopGS.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.Download, UserAction.Download.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.UpdateGame, UserAction.UpdateGame.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.UpdateGuard, UserAction.UpdateGuard.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.OpenGroup, UserAction.OpenGroup.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.CloseGroup, UserAction.CloseGroup.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.UploadServerConfigFile, UserAction.SendConfigFile.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.SwitchToTree, UserAction.SwitchToTree.ToString()));
        ListBoxOperation.Items.Add(new ListItem(StringDef.Abort, UserAction.Abort.ToString()));
        ListBoxOperation.SelectedIndex = 0;
    }

    protected void buttonOperation_Click(object sender, EventArgs e)
    {
        string[] groupid;
        try
        {
            if (ListBoxOperation.SelectedValue == UserAction.StartGame.ToString())
            {
                buttonStart();
            }
            else if (ListBoxOperation.SelectedValue == UserAction.StopGame.ToString())
            {
                buttonStop(GameServer.ServerType.unidentified);
            }
            else if (ListBoxOperation.SelectedValue == UserAction.StopGS.ToString())
            {
                buttonStop(GameServer.ServerType.gameserver);
            }
            else if (ListBoxOperation.SelectedValue == UserAction.Download.ToString())
            {
                buttonDownload();
            }
            else if (ListBoxOperation.SelectedValue == UserAction.UpdateGame.ToString())
            {
                buttonUpdateGame();
            }
            else if (ListBoxOperation.SelectedValue == UserAction.UpdateGuard.ToString())
            {
                UpdateGuard();
            }
            else if (ListBoxOperation.SelectedValue == UserAction.SendConfigFile.ToString())
            {
                SendConfigFile();
            }
            else if (ListBoxOperation.SelectedValue == UserAction.OpenGroup.ToString())
            {
                selectGroup = textboxServerGroupId.Text;
                    groupid = selectGroup.Split(';');
                    foreach (string gid in groupid)
                    {
                        if (gid == string.Empty) continue;
                        ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                        group.SetGroupEnable(ServerGroup.GroupEnable.Open);
                    }
            }
            else if (ListBoxOperation.SelectedValue == UserAction.CloseGroup.ToString())
            {
                selectGroup = textboxServerGroupId.Text;
                    groupid = selectGroup.Split(';');
                    foreach (string gid in groupid)
                    {
                        if (gid == string.Empty) continue;
                        ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(gid));
                        group.SetGroupEnable(ServerGroup.GroupEnable.Close);
                    }
            }
            else if (ListBoxOperation.SelectedValue == UserAction.SwitchToTree.ToString())
            {
                Page.Response.Redirect("~/updateserver/dispensefiles2.aspx");
            }
            else
            {
                    AbortAction();             
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            Util.DebugLog(ex.Message + Environment.NewLine + ex.StackTrace);
       
            textboxServerGroupId.Text = "";
            selectGroup = "";
            Timer_GroupStatus.Enabled = false;
            ActionResultList.Clear();
            ResultList.Rows.Clear();
            Session["JobType"] = null;
            Session["bCheckboxEnable"] = null;
            ViewState["SerialNum"] = null;
            ViewState["TimeStart"] = null;
        }
    }

    private void SendConfigFile()
    {
        jobType = UserAction.SendConfigFile;
        if (InitAction(GameServer.ServerType.unidentified))
        {
            SerialNum = AdminServer.TheInstance.GameServerManager.WriteServerOperationLog(CurrentUser.Id, ServerGroup.Type.Group, -2, GameServer.ServerOperationType.UpdateGuard, true, GetAllGroupFullName() + lblStartTime.Text, GetAllGroupAndFatherID());
            LabelSerialNum.Text = SerialNum.ToString();
            for (int j = 0; j < gameServers.Count; j++)
            {
                IList servers = (IList)gameServers[j];

                for (int i = 0; i < servers.Count; i++)
                {
                    GameServer server = servers[i] as GameServer;
                    if (server != null)
                    {
                        if (WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.EXECUTE, Session))
                        {
                            if (server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyReconfigureGuard))
                            {
                                WriteLog(server.Id, GameServer.ServerOperationType.UpdateConfigFile, true, "【" + server.IpAddress + "】" + StringDef.Success);
                            }
                            else
                            {
                                WriteLog(server.Id, GameServer.ServerOperationType.UpdateConfigFile, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.SendMessage);
                            }
                        }
                        else
                        {
                            WriteLog(server.Id, GameServer.ServerOperationType.UpdateConfigFile, false, "【" + server.IpAddress + "】" + StringDef.Failure + StringDef.Colon + StringDef.NotEnoughPrivilege);
                        }
                        servers[servers.IndexOf(server)] = null;
                    }
                }
            }
        }
    }
}
