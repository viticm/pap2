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

public partial class GameServer_EditServerGroup : BasePage
{
    String mark = "_";
    String rmark = "/";
    GameServer _server = null;


    protected void Page_Load(object sender, EventArgs e)
    {
        string strScript = "\n";
        if (!IsClientScriptBlockRegistered("clientScript"))
        {
            strScript = "<script>\n";
            strScript += "function OpenWin(st){\n";
            strScript += "var temp;\n";
            strScript += "var serverType;\n";
            strScript += "if(st=='Bishop'){\n";
            strScript += "var str=window.open('ServerConfig.aspx?serverType=Bishop');\n";
            strScript += "}\n";
            strScript += "if(st=='Goddess') {str=window.open('ServerConfig.aspx?serverType=Goddess')};\n";

            strScript += "if(st=='GameServer') {str=window.open('ServerConfig.aspx?serverType=GameServer')};\n";
            strScript += "if(st=='GameServer2') {str=window.open('ServerConfig.aspx?serverType=GameServer2')};\n";
            strScript += "if(st=='GameServer3') {str=window.open('ServerConfig.aspx?serverType=GameServer3')};\n";
            strScript += "if(st=='GameServer4') {str=window.open('ServerConfig.aspx?serverType=GameServer4')};\n";
            strScript += "if(st=='GameServer5') {str=window.open('ServerConfig.aspx?serverType=GameServer5')};\n";
            strScript += "if(st=='GameServer6') {str=window.open('ServerConfig.aspx?serverType=GameServer6')};\n";
            strScript += "if(st=='GameServer7') {str=window.open('ServerConfig.aspx?serverType=GameServer7')};\n";
            strScript += "if(st=='GameServer8') {str=window.open('ServerConfig.aspx?serverType=GameServer8')};\n";

            strScript += "if(st=='GameCenter') {str=window.open('ServerConfig.aspx?serverType=GameCenter')};\n";

            strScript += "if(st=='LogServer') {str=window.open('ServerConfig.aspx?serverType=LogServer')};\n";

            strScript += "if(str!=null) {str.moveTo(600,120);str.resizeTo(650,820);}\n";
            strScript += "}\n";
            strScript += "</script>\n";
            RegisterClientScriptBlock("clientScript", strScript);
        }

        ButtonShowBishopConfig.Attributes.Add("OnClick", "OpenWin('Bishop')");
        ButtonShowGoddessConfig.Attributes.Add("OnClick", "OpenWin('Goddess')");
        
        ButtonShowGameServerConfig.Attributes.Add("OnClick", "OpenWin('GameServer')");
        ButtonShowGameServerConfig2.Attributes.Add("OnClick", "OpenWin('GameServer2')");
        ButtonShowGameServerConfig3.Attributes.Add("OnClick", "OpenWin('GameServer3')");
        ButtonShowGameServerConfig4.Attributes.Add("OnClick", "OpenWin('GameServer4')");
        ButtonShowGameServerConfig5.Attributes.Add("OnClick", "OpenWin('GameServer5')");
        ButtonShowGameServerConfig6.Attributes.Add("OnClick", "OpenWin('GameServer6')");
        ButtonShowGameServerConfig7.Attributes.Add("OnClick", "OpenWin('GameServer7')");
        ButtonShowGameServerConfig8.Attributes.Add("OnClick", "OpenWin('GameServer8')");

        ButtonShowGameCenterConfig.Attributes.Add("OnClick", "OpenWin('GameCenter')");

        ButtonShowLogServerConfig.Attributes.Add("OnClick", "OpenWin('LogServer')");

        if (!Page.IsPostBack)
        {

            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerEditServerGroup, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            TextBoxFSEyeIp.Text = SystemConfig.Current.AdminServerListenIP;
            int defaultsystermip = SystemConfig.Current.AdminServerListenPort;
            TextBoxFSEyePort.Text = Convert.ToString(defaultsystermip);
            

            FtpDropDownList.ListFtpByType(FTPServer.FTPServerType.ServerUpdate);

            //根据选中的服务器组内已有的游戏服务器个数，存储本次应当生成的游戏服务器的游戏目录、名称、备注的数字编号，初次加载页面时值设置为-1
            ViewState["gameServerNumThisTime"] = null;

            //清除所有页面内的Session(如不清除会有问题的)
            if (Session["Bishop"] != null)
                Session["Bishop"] = null;
            if (Session["Goddess"] != null)
                Session["Goddess"] = null;
            if (Session["GameCenter"] != null)
                Session["GameCenter"] = null;
            if (Session["GameServer"] != null)
                Session["GameServer"] = null;
            if (Session["GameServer2"] != null)
                Session["GameServer2"] = null;
            if (Session["GameServer3"] != null)
                Session["GameServer3"] = null;
            if (Session["GameServer4"] != null)
                Session["GameServer4"] = null;
            if (Session["GameServer5"] != null)
                Session["GameServer5"] = null;
            if (Session["GameServer6"] != null)
                Session["GameServer6"] = null;
            if (Session["GameServer7"] != null)
                Session["GameServer7"] = null;
            if (Session["GameServer8"] != null)
                Session["GameServer8"] = null;
            if (Session["LogServer"] != null)
                Session["LogServer"] = null;
        }

        //提示没有配置文件
        //if (Session["GameServer"] == null)
        //    ButtonShowGameServerConfig.BackColor = System.Drawing.Color.Red;
        //else
        //    ButtonShowGameServerConfig.BackColor = System.Drawing.Color.Blue;

        //if (Session["GameCenter"] == null)
        //    ButtonShowGameCenterConfig.BackColor = System.Drawing.Color.Red;
        //else
        //    ButtonShowGameCenterConfig.BackColor = System.Drawing.Color.Blue;

        //if (Session["Goddess"] == null)
        //    ButtonShowGoddessConfig.BackColor = System.Drawing.Color.Red;
        //else
        //    ButtonShowGoddessConfig.BackColor = System.Drawing.Color.Blue;

        //if (Session["Bishop"] == null)
        //    ButtonShowBishopConfig.BackColor = System.Drawing.Color.Red;
        //else
        //    ButtonShowBishopConfig.BackColor = System.Drawing.Color.Blue;
    }

    protected void ShowGameServer()
    {
        
        int gameServerCount = int.Parse(GameServerCountDropDownList.SelectedValue);

        for (int i = 2; i <= gameServerCount; i++)
        {

            if (i == 2)
            {
                UpdatePanelGameServer2.Visible = true;
                CheckBoxGameServer2.Checked = true;
                CheckBoxGameServer2_Click(null, null);
            }
            if (i == 3)
            {
                UpdatePanelGameServer3.Visible = true;
                CheckBoxGameServer3.Checked = true;
                CheckBoxGameServer3_Click(null, null);
            }
            if (i == 4)
            {
                UpdatePanelGameServer4.Visible = true;
                CheckBoxGameServer4.Checked = true;
                CheckBoxGameServer4_Click(null, null);
            }
            if (i == 5)
            {
                UpdatePanelGameServer5.Visible = true;
                CheckBoxGameServer5.Checked = true;
                CheckBoxGameServer5_Click(null, null);
            }
            if (i == 6)
            {
                UpdatePanelGameServer6.Visible = true;
                CheckBoxGameServer6.Checked = true;
                CheckBoxGameServer6_Click(null, null);
            }
            if (i == 7)
            {
                UpdatePanelGameServer7.Visible = true;
                CheckBoxGameServer7.Checked = true;
                CheckBoxGameServer7_Click(null, null);
            }
            if (i == 8)
            {
                UpdatePanelGameServer8.Visible = true;
                CheckBoxGameServer8.Checked = true;
                CheckBoxGameServer8_Click(null, null);
            }
        }

    }

    //该函数在真正添加服务器之前对所有输入和配置进行检查
    private void CheckInputs(ref bool aSession, ref bool bSession, ref bool cSession)
    {
        
        CheckBox[] Svrs = new CheckBox[] {
            CheckBoxBishop,
            CheckBoxGoddess,
            CheckBoxGameCenter,
            CheckBoxGameServer1,
            CheckBoxGameServer2,
            CheckBoxGameServer3,
            CheckBoxGameServer4,
            CheckBoxGameServer5,
            CheckBoxGameServer6,
            CheckBoxGameServer7,
            CheckBoxGameServer8,
            CheckBoxLogServer };

        TextBox[] SvrNames = new TextBox[] {
            TextBoxBishopName,
            TextBoxGoddessName,
            TextBoxGameCenterName,
            TextBoxGameServerName1, 
            TextBoxGameServerName2, 
            TextBoxGameServerName3, 
            TextBoxGameServerName4,
            TextBoxGameServerName5,
            TextBoxGameServerName6,
            TextBoxGameServerName7,
            TextBoxGameServerName8,
            TextBoxLogServerName };

        LinkButton[] LinkButtons = new LinkButton[] {
            ButtonShowBishopConfig,
            ButtonShowGoddessConfig,
            ButtonShowGameCenterConfig,
            ButtonShowGameServerConfig,
            ButtonShowGameServerConfig2,
            ButtonShowGameServerConfig3,
            ButtonShowGameServerConfig4,
            ButtonShowGameServerConfig5,
            ButtonShowGameServerConfig6,
            ButtonShowGameServerConfig7,
            ButtonShowGameServerConfig8,
            ButtonShowLogServerConfig };


        //先将之前有可能设置为红色的控件恢复原来的颜色
        for (int i = 0; i < SvrNames.Length; i++)
        {
            SvrNames[i].BackColor = System.Drawing.Color.Empty;
            LinkButtons[i].BackColor = System.Drawing.Color.Empty;
        }

        //对检查用户输入的各服务器名称是否存在相同
        for (int i = 0; i < Svrs.Length; i++)
        {
            if (Svrs[i].Checked == true)
            {
                for (int j = i + 1; j < Svrs.Length; j++)
                {
                    if (Svrs[j].Checked == true)
                    {
                        if (SvrNames[i].Text == SvrNames[j].Text)
                        {
                            cSession = false;

                            SvrNames[i].BackColor = System.Drawing.Color.Red;
                            SvrNames[j].BackColor = System.Drawing.Color.Red;

                            LabelOpMsg.Text = String.Empty;
                            LabelOpMsg.Text += "\n添加终止：在你的输入中，存在服务器名称相同的情况，这是不允许的\n";
                            LabelOpMsg.Visible = true;

                            return;
                        }
                    }
                }
            }
        }


        
        
        //检查bishop
        if (CheckBoxBishop.Checked)
        {
            if (serverNameConflict(TextBoxBishopName.Text))
            {
                aSession = false;
                TextBoxBishopName.BackColor = System.Drawing.Color.Red;
            }
            
            if (Session["Bishop"] == null)
            {
                bSession = false;
                ButtonShowBishopConfig.BackColor = System.Drawing.Color.Red;
            }
        }

        //检查goddess
        if (CheckBoxGoddess.Checked)
        {
            if (serverNameConflict(TextBoxGoddessName.Text))
            {
                aSession = false;
                TextBoxGoddessName.BackColor = System.Drawing.Color.Red;
            }
            
            if (Session["Goddess"] == null)
            {
                bSession = false;
                ButtonShowGoddessConfig.BackColor = System.Drawing.Color.Red;
            }
        }

        //检查gamecenter
        if (CheckBoxGameCenter.Checked)
        {
            if (serverNameConflict(TextBoxGameCenterName.Text))
            {
                aSession = false;
                TextBoxGameCenterName.BackColor = System.Drawing.Color.Red;
            }
            
            if (Session["GameCenter"] == null)
            {
                bSession = false;
                ButtonShowGameCenterConfig.BackColor = System.Drawing.Color.Red;
            }
        }

        //检查logserver
        if (CheckBoxLogServer.Checked)
        {
            if (serverNameConflict(TextBoxLogServerName.Text))
            {
                aSession = false;
                TextBoxLogServerName.BackColor = System.Drawing.Color.Red;
            }

            if (Session["LogServer"] == null)
            {
                bSession = false;
                ButtonShowLogServerConfig.BackColor = System.Drawing.Color.Red;
            }
        }

        //检查gameservers
        CheckBox[] gameSvr = new CheckBox[] {
            CheckBoxGameServer1,
            CheckBoxGameServer2,
            CheckBoxGameServer3,
            CheckBoxGameServer4,
            CheckBoxGameServer5,
            CheckBoxGameServer6,
            CheckBoxGameServer7,
            CheckBoxGameServer8 };

        for (int i = 0; i < gameSvr.Length; i++)
        {
            if (gameSvr[i].Checked)
            {
                if (i == 0)
                {
                    if (serverNameConflict(TextBoxGameServerName1.Text))
                    {
                        aSession = false;
                        TextBoxGameServerName1.BackColor = System.Drawing.Color.Red;
                    }
                    
                    if (Session["GameServer"] == null)
                    {
                        bSession = false;
                        ButtonShowGameServerConfig.BackColor = System.Drawing.Color.Red;
                    }
                }
                else
                {
                    string tempTextBoxString = "TextBoxGameServerName" + (i + 1).ToString();
                    TextBox tempTextBox = (TextBox)UpdatePanel2.FindControl(tempTextBoxString);
                    
                    if (serverNameConflict(tempTextBox.Text))
                    {
                        aSession = false;
                        tempTextBox.BackColor = System.Drawing.Color.Red;
                    }
                    
                    string tempcheckBoxString = "gameServerConfigCheckBox" + (i + 1).ToString();
                    CheckBox tempCheckBox = (CheckBox)UpdatePanel2.FindControl(tempcheckBoxString);
                    string tempSessionString = "GameServer" + (i + 1).ToString();
                    string tempLinkButtonString = "ButtonShowGameServerConfig" + (i + 1).ToString();
                    LinkButton tempLinkButton = (LinkButton)UpdatePanel2.FindControl(tempLinkButtonString);

                    if (tempCheckBox.Checked == false)
                    {
                        if (Session[tempSessionString] == null)
                        {
                            bSession = false;
                            tempLinkButton.BackColor = System.Drawing.Color.Red;
                        }
                    }
                }
            }
        }

        LabelOpMsg.Text = String.Empty;

        if (aSession == false)
        {
            LabelOpMsg.Text += "\n添加终止：存在服务器名称冲突\n";
            LabelOpMsg.Visible = true;
        }
        
        if (bSession == false)
        {
            LabelOpMsg.Text += "\n添加终止：存在服务器未设定配置文件\n";
            LabelOpMsg.Visible = true;
        }
    }

    protected void LinkButtonSave_Click(object sender, EventArgs e)
    {
        //服务器名称冲突的标志
        bool aSession = true;
        
        bool bSession = true;

        bool cSession = true;

        CheckInputs(ref aSession, ref bSession, ref cSession);

        if ((aSession == false) || (bSession == false) || (cSession == false))
            return;

        LabelSuccess.Text = "";
        LabelOpMsg.Text = "";
        ArrayList servers = new ArrayList();

        CheckBox[] gameSvr = new CheckBox[] {
            CheckBoxGameServer1,
            CheckBoxGameServer2,
            CheckBoxGameServer3,
            CheckBoxGameServer4,
            CheckBoxGameServer5,
            CheckBoxGameServer6,
            CheckBoxGameServer7,
            CheckBoxGameServer8 };
        TextBox[] gameSvrName = new TextBox[] { 
            TextBoxGameServerName1, 
            TextBoxGameServerName2, 
            TextBoxGameServerName3, 
            TextBoxGameServerName4,
            TextBoxGameServerName5,
            TextBoxGameServerName6,
            TextBoxGameServerName7,
            TextBoxGameServerName8 };
        TextBox[] gamSvrComment = new TextBox[] {
            TextBoxGameServerComment1,
            TextBoxGameServerComment2,
            TextBoxGameServerComment3,
            TextBoxGameServerComment4,
            TextBoxGameServerComment5,
            TextBoxGameServerComment6, 
            TextBoxGameServerComment7, 
            TextBoxGameServerComment8 };
        TextBox[] gameSvrIpAddress = new TextBox[]{
            TextBoxGameServerIpAddress1,
            TextBoxGameServerIpAddress2,
            TextBoxGameServerIpAddress3,
            TextBoxGameServerIpAddress4,
            TextBoxGameServerIpAddress5,
            TextBoxGameServerIpAddress6,
            TextBoxGameServerIpAddress7,
            TextBoxGameServerIpAddress8
        };
        TextBox[] gameServerDir = new TextBox[]{
            TextBoxGameServerDir1,
            TextBoxGameServerDir2,
            TextBoxGameServerDir3,
            TextBoxGameServerDir4,
            TextBoxGameServerDir5,
            TextBoxGameServerDir6,
            TextBoxGameServerDir7,
            TextBoxGameServerDir8
        };
        
        try
        {
            ////权限检查

            //if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
            //{
            //    LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            //    return;
            //}

            //else
            //{
            //    LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            //    return;
            //}
            //换取服务器组
            ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;

            string strName = "";
            string strComment = "";
            string strIP = "";
            string strDir = "";
            string serverConfigFile = "";



            //添加bishop
            if (CheckBoxBishop.Checked)
            {
                strName = TextBoxBishopName.Text;
                strComment = TextBoxBishopComment.Text;
                strIP = TextBoxBishopIpAddress.Text;
                strDir = CheckGameDir(TextBoxBishopDir.Text);
                serverConfigFile = (string)Session["Bishop"];

                //配置lord                

                TheAdminServer.GameServerManager.AddGameServer(strName, strComment, strIP, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.bishop,FtpDropDownList.SelectFtp);
                GameServer svr = getGameServerByName(strName);
                servers.Add(svr);
            }

            //添加goddess
            if (CheckBoxGoddess.Checked)
            {
                strName = TextBoxGoddessName.Text;
                strComment = TextBoxGoddessComment.Text;
                strIP = TextBoxGoddessIpAddress.Text;
                strDir = CheckGameDir(TextBoxGoddessDir.Text);
                serverConfigFile = (string)Session["Goddess"];

                TheAdminServer.GameServerManager.AddGameServer(strName, strComment, strIP, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.goddess, FtpDropDownList.SelectFtp);
                GameServer svr = getGameServerByName(strName);

                servers.Add(svr);
            }

            //添加gamecenter
            if (CheckBoxGameCenter.Checked)
            {
                strName = TextBoxGameCenterName.Text;
                strComment = TextBoxGameCenterComment.Text;
                strIP = TextBoxGameCenterIpAddress.Text;
                strDir = CheckGameDir(TextBoxGameCenterDir.Text);
                serverConfigFile = (string)Session["GameCenter"];

                TheAdminServer.GameServerManager.AddGameServer(strName, strComment, strIP, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.gamecenter, FtpDropDownList.SelectFtp);
                GameServer svr = getGameServerByName(strName);

                servers.Add(svr);
            }

            //添加logserver
            if (CheckBoxLogServer.Checked)
            {
                strName = TextBoxLogServerName.Text;
                strComment = TextBoxLogServerComment.Text;
                strIP = TextBoxLogServerIpAddress.Text;
                strDir = CheckGameDir(TextBoxLogServerDir.Text);
                serverConfigFile = (string)Session["LogServer"];

                TheAdminServer.GameServerManager.AddGameServer(strName, strComment, strIP, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.logserver, FtpDropDownList.SelectFtp);
                GameServer svr = getGameServerByName(strName);

                servers.Add(svr);
            }

            //添加gameservers
            for (int i = 0; i < gameSvr.Length; i++)
            {
                if (gameSvr[i].Checked)
                {
                    strName = gameSvrName[i].Text;
                    strComment = gamSvrComment[i].Text;
                    strIP = gameSvrIpAddress[i].Text;
                    strDir = CheckGameDir(gameServerDir[i].Text);

                    if (i == 0)
                    {
                        serverConfigFile = (string)Session["GameServer"];
                    }
                    else
                    {
                        string tempcheckBoxString = "gameServerConfigCheckBox" + (i + 1).ToString();
                        string tempSessionString = "GameServer" + (i+1).ToString();
                        CheckBox tempCheckBox = (CheckBox)UpdatePanel2.FindControl(tempcheckBoxString);

                        if (tempCheckBox.Checked == true)
                        {
                            serverConfigFile = (string)Session["GameServer"];
                        }
                        else
                        {
                            serverConfigFile = (string)Session[tempSessionString];
                        }
                    }

                    TheAdminServer.GameServerManager.AddGameServer(strName, strComment, strIP, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.gameserver, FtpDropDownList.SelectFtp);
                    GameServer svr = getGameServerByName(strName);

                    servers.Add(svr); ;
                }

            }

            if (servers.Count == 0)
            {
                //LabelOpMsg.Text = StringDef.AddServer + StringDef.Failure + StringDef.Colon + StringDef.ConfigFile + StringDef.NotExist;
                
                LabelOpMsg.Text = StringDef.AddServer + StringDef.Failure + StringDef.Colon + "未指定任何要添加的服务器";

                LabelOpMsg.Visible = true;
            }
            else
            {
                foreach (GameServer svr in servers)
                {
                    LordConfig lordConfig = GuardCfg(svr.IpAddress);
                    svr.LordConfig.ServerCount = lordConfig.ServerCount;
                    svr.LordConfig.ServerIDs = lordConfig.ServerIDs;
                    svr.LordConfig.BindIp = svr.IpAddress;
                    
                    //本组添加的有GameCenter的情况下，
                    //只对GameCenter的LordConfig添加GMCIp和GMCAccount的信息
                    if (CheckBoxGameCenter.Checked)
                    {
                        if (svr.Type == GameServer.ServerType.gamecenter)
                        {
                            if (GMCIpTextBox.Text.Length > 0)
                            {
                                lordConfig.GMCIp = GMCIpTextBox.Text;
                            }

                            if (GMCAccountTextBox.Text.Length > 0)
                            {
                                lordConfig.GMCAccount = GMCAccountTextBox.Text;
                            }
                        }
                    }
                    TheAdminServer.GameServerManager.ModifyGameServer(svr, svr.Name, svr.Comment, svr.IpAddress, svr.GameDir, svr.Group, lordConfig, GameServer.ConfigGuardState.Configuring, svr.ConfigContent, svr.Type, FtpDropDownList.SelectFtp);
                    SendGuardCfg(svr);
                }
                LabelSuccess.Text = StringDef.AddServer + StringDef.Colon + StringDef.Ok;
                LabelSuccess.Visible = true;

                Session["Bishop"] = null;
                Session["Goddess"] = null;
                Session["GameCenter"] = null;
                Session["GameServer"] = null;
                Session["GameServer2"] = null;
                Session["GameServer3"] = null;
                Session["GameServer4"] = null;
                Session["GameServer5"] = null;
                Session["GameServer6"] = null;
                Session["GameServer7"] = null;
                Session["GameServer8"] = null;
                Session["LogServer"] = null;
                LinkButtonEdit.Enabled = false;
            }
        }
        catch (Exception ex)
        {
            foreach (GameServer svr in servers)
            {
                TheAdminServer.GameServerManager.DeleteGameServer(svr);
            }
            LabelOpMsg.Text = StringDef.AddServer + StringDef.Failure + StringDef.Colon + ex.Message;
            LabelOpMsg.Visible = true;
        }
    }

    //从名字获取server
    protected GameServer getGameServerByName(string gamesevername)
    {
        foreach (GameServer server in TheAdminServer.GameServerManager.Servers)
        {
            if (gamesevername == server.Name)
            {
                return server;
            }
        }
        return null;
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
        }
        return serverConfigFileName;
    }

    private void SendFile(string Content, string RemoteFile)
    {
        string[] para = new string[3];
        para[0] = _server.Id.ToString();
        para[1] = Content;
        para[2] = RemoteFile;

        if (_server.DoPlugInAction(CurrentUser.Id, E2gSendfile.PlugInGuid, E2gSendfile.actionKey, para))
        {
            LabelSuccess.Text = StringDef.Success; //+ "<br />";
            LabelSuccess.Visible = true;
        }
        else
        {
            LabelOpMsg.Text = StringDef.Failure;
        }
        para = null;
    }

    //重用自EditServer.aspx.cs
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


    //修改并重用自EditServer.aspx.cs
    //设置游戏目录、服务器名称和备注的textbox，调用了函数 SetServerNameAndComment
    private void SetGameDirAndServerNameAndComment(TextBox TextBoxGameDir, TextBox TextBoxServerName, TextBox TextBoxServerComment, TextBox TextBoxServerId, GameServer.ServerType ServerType, string ServerTypeStringCHS)
    {
        //若这不是初次勾选gameserver的checkbox，则已经无需计算服务器组中已有的游戏服务器个数了
        if ((ServerType == GameServer.ServerType.gameserver) && (ViewState["gameServerNumThisTime"] != null))
        {
            int gameServerNumThisTime = (int)ViewState["gameServerNumThisTime"];
            string gameServerNumThisTimeString = gameServerNumThisTime.ToString();
            if (gameServerNumThisTime < 10)
            {
                gameServerNumThisTimeString = "0" + gameServerNumThisTimeString;
            }
            TextBoxGameDir.Text = "../" + Enum.GetName(typeof(GameServer.ServerType), ServerType) + gameServerNumThisTimeString + "/";
            SetServerNameAndComment(gameServerNumThisTime, ServerTypeStringCHS, TextBoxServerName, TextBoxServerComment);
            gameServerNumThisTime++;
            ViewState["gameServerNumThisTime"] = gameServerNumThisTime;
            return;
        }

        string GameDir = "../";
        int i = 1;
        bool atLeaseOne = false;
        string maxNum;      //存储文件夹编号的最大值        
        string tempbit;     //文件夹编号的数字多于1位时，存储多读的那1位
        GameDir += Enum.GetName(typeof(GameServer.ServerType), ServerType);
        ServerGroup svrGroup = ServerGroupDropDownList.SelectedServerGroup;
        foreach (GameServer childServer in svrGroup.List)
        {
            int j = childServer.GameDir.Length - 2;

            if (childServer.Id.ToString() != TextBoxServerId.Text)
            {
                if (childServer.Type == ServerType)
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
            if (ServerType == GameServer.ServerType.gameserver)
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
            if (ServerType == GameServer.ServerType.gameserver)
            {
                GameDir += "01";
            }
        }

        GameDir += "/";
        TextBoxGameDir.Text = GameDir;

        if (ServerType == GameServer.ServerType.gameserver)
        {
            ViewState["gameServerNumThisTime"] = i + 1;
        }

        SetServerNameAndComment(i, ServerTypeStringCHS, TextBoxServerName, TextBoxServerComment);

        
    }

    //设置服务器名称和备注的textbox
    private void SetServerNameAndComment(int num, string ServerType, TextBox TextBoxServerName, TextBox TextBoxServerComment)
    {
        string tempString = ServerGroupDropDownList.SelectedServerGroup.Name + "_" + ServerType;
        if (ServerType == StringDef.ServerTypeGameServer)
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


    //服务器CheckBox
    protected void CheckBoxBishop_Click(object sender, EventArgs e)
    {
        TextBoxBishopId.Text = StringDef.Unknown;
        PanelBishop.Enabled = CheckBoxBishop.Checked;
        RequiredFieldValidator13.Enabled = CheckBoxBishop.Checked;
        RequiredFieldValidator.Enabled = CheckBoxBishop.Checked;
        ServerIpValidator.Enabled = CheckBoxBishop.Checked;
        
        if (CheckBoxBishop.Checked == true)
            SetGameDirAndServerNameAndComment(TextBoxBishopDir, TextBoxBishopName, TextBoxBishopComment, TextBoxBishopId, GameServer.ServerType.bishop, StringDef.ServerTyperBishop);

        if (CheckBoxBishop.Checked == false)
        {
            TextBoxBishopName.BackColor = System.Drawing.Color.Empty;
            ButtonShowBishopConfig.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGoddess_Click(object sender, EventArgs e)
    {
        TextBoxGoddessId.Text = StringDef.Unknown;
        PanelGoddess.Enabled = CheckBoxGoddess.Checked;
        RequiredFieldValidator14.Enabled = CheckBoxGoddess.Checked;
        RequiredFieldValidator1.Enabled = CheckBoxGoddess.Checked;
        ServerIpValidator1.Enabled = CheckBoxGoddess.Checked;

        if (CheckBoxGoddess.Checked == true)
            SetGameDirAndServerNameAndComment(TextBoxGoddessDir, TextBoxGoddessName, TextBoxGoddessComment, TextBoxGoddessId, GameServer.ServerType.goddess, StringDef.ServerTyperGoddess);

        if (CheckBoxGoddess.Checked == false)
        {
            TextBoxGoddessName.BackColor = System.Drawing.Color.Empty;
            ButtonShowGoddessConfig.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameCenter_Click(object sender, EventArgs e)
    {
        TextBoxGameCenterId.Text = StringDef.Unknown;
        PanelGameCenter.Enabled = CheckBoxGameCenter.Checked;
        RequiredFieldValidator15.Enabled = CheckBoxGameCenter.Checked;
        RequiredFieldValidator2.Enabled = CheckBoxGameCenter.Checked;
        ServerIpValidator2.Enabled = CheckBoxGameCenter.Checked;
        RequiredFieldValidator24.Enabled = CheckBoxGameCenter.Checked;
        RegularExpressionValidator1.Enabled = CheckBoxGameCenter.Checked;
        RequiredFieldValidator25.Enabled = CheckBoxGameCenter.Checked;

        if (CheckBoxGameCenter.Checked == true)
            SetGameDirAndServerNameAndComment(TextBoxGameCenterDir, TextBoxGameCenterName, TextBoxGameCenterComment, TextBoxGameCenterId, GameServer.ServerType.gamecenter, StringDef.ServerTyperGameCenter);

        if (CheckBoxGameCenter.Checked == false)
        {
            TextBoxGameCenterName.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameCenterConfig.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxLogServer_Click(object sender, EventArgs e)
    {
        TextBoxLogServerId.Text = StringDef.Unknown;
        PanelLogServer.Enabled = CheckBoxLogServer.Checked;
        RequiredFieldValidator26.Enabled = CheckBoxLogServer.Checked;
        RequiredFieldValidator27.Enabled = CheckBoxLogServer.Checked;
        RegularExpressionValidator2.Enabled = CheckBoxLogServer.Checked;

        if (CheckBoxLogServer.Checked == true)
            SetGameDirAndServerNameAndComment(TextBoxLogServerDir, TextBoxLogServerName, TextBoxLogServerComment, TextBoxLogServerId, GameServer.ServerType.logserver, StringDef.ServerTypeLogServer);
        if (CheckBoxLogServer.Checked == false)
        {
            TextBoxLogServerName.BackColor = System.Drawing.Color.Empty;
            ButtonShowLogServerConfig.BackColor = System.Drawing.Color.Empty;
        }

    }
    protected void CheckBoxGameServer1_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId1.Text = StringDef.Unknown;
        PanelGameServer1.Enabled = CheckBoxGameServer1.Checked;
        RequiredFieldValidator16.Enabled = CheckBoxGameServer1.Checked;
        RequiredFieldValidator3.Enabled = CheckBoxGameServer1.Checked;
        ServerIpValidator3.Enabled = CheckBoxGameServer1.Checked;

        CheckBox[] gameSvr = new CheckBox[] {
        CheckBoxGameServer1,
        CheckBoxGameServer2,
        CheckBoxGameServer3,
        CheckBoxGameServer4,
        CheckBoxGameServer5,
        CheckBoxGameServer6,
        CheckBoxGameServer7,
        CheckBoxGameServer8 };


        if (CheckBoxGameServer1.Checked == true)
        {
            if (TextBoxGameServerDir1.Text == String.Empty)
            {
                SetGameDirAndServerNameAndComment(TextBoxGameServerDir1, TextBoxGameServerName1, TextBoxGameServerComment1, TextBoxGameServerId1, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);
            }
        }
        else
        {
            TextBoxGameServerName1.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig.BackColor = System.Drawing.Color.Empty;
            
            for (int i = 0; i < gameSvr.Length; i++)
            {
                if (gameSvr[i].Checked == true)
                {
                    gameSvr[i].Checked = false;
                    switch (i)
                    {
                        case 1:
                            CheckBoxGameServer2_Click(null, null);
                            break;
                        case 2:
                            CheckBoxGameServer3_Click(null, null);
                            break;
                        case 3:
                            CheckBoxGameServer4_Click(null, null);
                            break;
                        case 4:
                            CheckBoxGameServer5_Click(null, null);
                            break;
                        case 5:
                            CheckBoxGameServer6_Click(null, null);
                            break;
                        case 6:
                            CheckBoxGameServer7_Click(null, null);
                            break;
                        case 7:
                            CheckBoxGameServer8_Click(null, null);
                            break;
                    }
                }
            }
        }
    }
  
    protected void CheckBoxGameServer2_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId2.Text = StringDef.Unknown;
        PanelGameServer2.Enabled = CheckBoxGameServer2.Checked;
        RequiredFieldValidator17.Enabled = CheckBoxGameServer2.Checked;
        RequiredFieldValidator4.Enabled = CheckBoxGameServer2.Checked;
        ServerIpValidator4.Enabled = CheckBoxGameServer2.Checked;

        if ((CheckBoxGameServer2.Checked == true) && (TextBoxGameServerDir2.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir2, TextBoxGameServerName2, TextBoxGameServerComment2, TextBoxGameServerId2, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer2.Checked == false)
        {
            TextBoxGameServerName2.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig2.BackColor = System.Drawing.Color.Empty;
        }
    }

    protected void CheckBoxGameServer3_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId3.Text = StringDef.Unknown;
        PanelGameServer3.Enabled = CheckBoxGameServer3.Checked;
        RequiredFieldValidator18.Enabled = CheckBoxGameServer3.Checked;
        RequiredFieldValidator5.Enabled = CheckBoxGameServer3.Checked;
        ServerIpValidator5.Enabled = CheckBoxGameServer3.Checked;

        if ((CheckBoxGameServer3.Checked == true) && (TextBoxGameServerDir3.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir3, TextBoxGameServerName3, TextBoxGameServerComment3, TextBoxGameServerId3, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer3.Checked == false)
        {
            TextBoxGameServerName3.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig3.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameServer4_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId4.Text = StringDef.Unknown;
        PanelGameServer4.Enabled = CheckBoxGameServer4.Checked;
        RequiredFieldValidator19.Enabled = CheckBoxGameServer4.Checked;
        RequiredFieldValidator6.Enabled = CheckBoxGameServer4.Checked;
        ServerIpValidator6.Enabled = CheckBoxGameServer4.Checked;

        if ((CheckBoxGameServer4.Checked == true) && (TextBoxGameServerDir4.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir4, TextBoxGameServerName4, TextBoxGameServerComment4, TextBoxGameServerId4, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer4.Checked == false)
        {
            TextBoxGameServerName4.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig4.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameServer5_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId5.Text = StringDef.Unknown;
        PanelGameServer5.Enabled = CheckBoxGameServer5.Checked;
        RequiredFieldValidator20.Enabled = CheckBoxGameServer5.Checked;
        RequiredFieldValidator7.Enabled = CheckBoxGameServer5.Checked;
        ServerIpValidator7.Enabled = CheckBoxGameServer5.Checked;

        if ((CheckBoxGameServer5.Checked == true) && (TextBoxGameServerDir5.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir5, TextBoxGameServerName5, TextBoxGameServerComment5, TextBoxGameServerId5, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer5.Checked == false)
        {
            TextBoxGameServerName5.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig5.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameServer6_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId6.Text = StringDef.Unknown;
        PanelGameServer6.Enabled = CheckBoxGameServer6.Checked;
        RequiredFieldValidator21.Enabled = CheckBoxGameServer6.Checked;
        RequiredFieldValidator8.Enabled = CheckBoxGameServer6.Checked;
        ServerIpValidator8.Enabled = CheckBoxGameServer6.Checked;

        if ((CheckBoxGameServer6.Checked == true) && (TextBoxGameServerDir6.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir6, TextBoxGameServerName6, TextBoxGameServerComment6, TextBoxGameServerId6, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer6.Checked == false)
        {
            TextBoxGameServerName6.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig6.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameServer7_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId7.Text = StringDef.Unknown;
        PanelGameServer7.Enabled = CheckBoxGameServer7.Checked;
        RequiredFieldValidator22.Enabled = CheckBoxGameServer7.Checked;
        RequiredFieldValidator9.Enabled = CheckBoxGameServer7.Checked;
        ServerIpValidator9.Enabled = CheckBoxGameServer7.Checked;

        if ((CheckBoxGameServer7.Checked == true) && (TextBoxGameServerDir7.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir7, TextBoxGameServerName7, TextBoxGameServerComment7, TextBoxGameServerId7, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer7.Checked == false)
        {
            TextBoxGameServerName7.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig7.BackColor = System.Drawing.Color.Empty;
        }
    }
    protected void CheckBoxGameServer8_Click(object sender, EventArgs e)
    {
        TextBoxGameServerId8.Text = StringDef.Unknown;
        PanelGameServer8.Enabled = CheckBoxGameServer8.Checked;
        RequiredFieldValidator23.Enabled = CheckBoxGameServer8.Checked;
        RequiredFieldValidator10.Enabled = CheckBoxGameServer8.Checked;
        ServerIpValidator10.Enabled = CheckBoxGameServer8.Checked;

        if ((CheckBoxGameServer8.Checked == true) && (TextBoxGameServerDir8.Text == String.Empty))
            SetGameDirAndServerNameAndComment(TextBoxGameServerDir8, TextBoxGameServerName8, TextBoxGameServerComment8, TextBoxGameServerId8, GameServer.ServerType.gameserver, StringDef.ServerTypeGameServer);

        if (CheckBoxGameServer8.Checked == false)
        {
            TextBoxGameServerName8.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig8.BackColor = System.Drawing.Color.Empty;
        }
    }

    protected void ServerGroupDropDownList_Changed(object sender, EventArgs e)
    {
        LabelOpMsg.Visible = false;


        if (CheckBoxBishop.Checked == true)
        {
            SetGameDirAndServerNameAndComment(TextBoxBishopDir, TextBoxBishopName, TextBoxBishopComment, TextBoxBishopId, GameServer.ServerType.bishop, StringDef.ServerTyperBishop);
            TextBoxBishopName.BackColor = System.Drawing.Color.Empty;
            ButtonShowBishopConfig.BackColor = System.Drawing.Color.Empty;
        }

        if (CheckBoxGoddess.Checked == true)
        {
            SetGameDirAndServerNameAndComment(TextBoxGoddessDir, TextBoxGoddessName, TextBoxGoddessComment, TextBoxGoddessId, GameServer.ServerType.goddess, StringDef.ServerTyperGoddess);
            TextBoxGoddessName.BackColor = System.Drawing.Color.Empty;
            ButtonShowGoddessConfig.BackColor = System.Drawing.Color.Empty;
        }

        if (CheckBoxGameCenter.Checked == true)
        {
            SetGameDirAndServerNameAndComment(TextBoxGameCenterDir, TextBoxGameCenterName, TextBoxGameCenterComment, TextBoxGameCenterId, GameServer.ServerType.gamecenter, StringDef.ServerTyperGameCenter);
            TextBoxGameCenterName.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameCenterConfig.BackColor = System.Drawing.Color.Empty;
        }

        if (CheckBoxLogServer.Checked == true)
        {
            SetGameDirAndServerNameAndComment(TextBoxLogServerDir, TextBoxLogServerName, TextBoxLogServerComment, TextBoxLogServerId, GameServer.ServerType.logserver, StringDef.ServerTypeLogServer);
            TextBoxLogServerName.BackColor = System.Drawing.Color.Empty;
            ButtonShowLogServerConfig.BackColor = System.Drawing.Color.Empty;
        }

        ViewState["gameServerNumThisTime"] = null;

        CheckBox[] gameSvr = new CheckBox[] {
            CheckBoxGameServer1,
            CheckBoxGameServer2,
            CheckBoxGameServer3,
            CheckBoxGameServer4,
            CheckBoxGameServer5,
            CheckBoxGameServer6,
            CheckBoxGameServer7,
            CheckBoxGameServer8 };

        TextBox[] gameServerDir = new TextBox[]{
            TextBoxGameServerDir1,
            TextBoxGameServerDir2,
            TextBoxGameServerDir3,
            TextBoxGameServerDir4,
            TextBoxGameServerDir5,
            TextBoxGameServerDir6,
            TextBoxGameServerDir7,
            TextBoxGameServerDir8 };

        for (int i = 0; i < gameSvr.Length; i++)
        {
            gameServerDir[i].Text = String.Empty;
            
            if (gameSvr[i].Checked == true)
            {
                gameSvr[i].Checked = false;
                switch (i)
                {
                    case 0:
                        CheckBoxGameServer1_Click(null, null);
                        break;
                    case 1:
                        CheckBoxGameServer2_Click(null, null);
                        break;
                    case 2:
                        CheckBoxGameServer3_Click(null, null);
                        break;
                    case 3:
                        CheckBoxGameServer4_Click(null, null);
                        break;
                    case 4:
                        CheckBoxGameServer5_Click(null, null);
                        break;
                    case 5:
                        CheckBoxGameServer6_Click(null, null);
                        break;
                    case 6:
                        CheckBoxGameServer7_Click(null, null);
                        break;
                    case 7:
                        CheckBoxGameServer8_Click(null, null);
                        break;
                }
            }
        }

        UpdatePanel[] gameServerUpdatePanel = new UpdatePanel[]{
            UpdatePanelGameServer2,
            UpdatePanelGameServer3,
            UpdatePanelGameServer4,
            UpdatePanelGameServer5,
            UpdatePanelGameServer6,
            UpdatePanelGameServer7,
            UpdatePanelGameServer8, };

        for (int i = 0; i < gameServerUpdatePanel.Length; i++)
        {
            if (gameServerUpdatePanel[i].Visible == true)
            {
                gameServerUpdatePanel[i].Visible = false;
            }
        }
    }

    //游戏目录最后一个“/”必须有
    protected string CheckGameDir(string gameDir)
    {
        string gameDirRes = gameDir;
        if (gameDir != null && gameDir.Length > 0)
            if (!gameDir.EndsWith(GameServer.FilePathDelimiter) && !gameDir.EndsWith(GameServer.FilePathWinDelimiter))
            {
                if (gameDir.IndexOf(GameServer.FilePathWinDelimiter) >= 0)
                {
                    gameDir = gameDir + GameServer.FilePathWinDelimiter;
                }
                else
                {
                    gameDir = gameDir + GameServer.FilePathDelimiter;
                }
                gameDirRes = gameDir;
            }
        return gameDirRes;
    }

    //配置fseye， 与DB数据库
    private LordConfig GuardCfg(string ipAddr)
    {
        int iServerCount = 0;
        string sServerIDs = "";
        LordConfig lordConfig = new LordConfig();
        foreach (GameServer server in AdminServer.TheInstance.GameServerManager.Servers)
        {
            if (server.IpAddress == ipAddr)
            {
                if (iServerCount == 0)
                    sServerIDs = server.Id.ToString();
                else
                    sServerIDs = sServerIDs + "," + server.Id.ToString();

                iServerCount++;
            }
        }

        lordConfig.ServerCount = iServerCount;
        lordConfig.ServerIDs = sServerIDs;
        
       
        lordConfig.FSEyeIp = TextBoxFSEyeIp.Text;
        lordConfig.FSEyePort = int.Parse(TextBoxFSEyePort.Text);
        return lordConfig;
    }

    //添加一个服务器函数
    protected Boolean addServer(string name, string comment, string ip, string gameDir, ServerGroup group, GameServer.ConfigGuardState configGuardState, string serverConfigFile, GameServer.ServerType serverType,int ftpServerID)
    {
        if (TheAdminServer.GameServerManager.AddGameServer(name, comment, ip, gameDir, group, configGuardState, serverConfigFile, serverType,ftpServerID))
        {
            LabelSuccess.Visible = true;
            LabelSuccess.Text = StringDef.AddGameServer + StringDef.Colon + StringDef.Success;
            return true;
        }
        else
        {
            LabelOpMsg.Text = StringDef.AddGameServer + StringDef.Colon + StringDef.Failure;
            return false;
        }
    }
    protected Boolean modifyServer(GameServer server, string name, string comment, string ip, string gameDir, ServerGroup group, LordConfig lordConfig, GameServer.ConfigGuardState ConfigState, string serverConfigFile, GameServer.ServerType serverType, int ftpServerID)
    {
        if (TheAdminServer.GameServerManager.ModifyGameServer(server, name, comment, ip, gameDir, group, lordConfig, server.ConfigState, serverConfigFile, serverType, ftpServerID))
        {
            LabelSuccess.Visible = true;
            LabelSuccess.Text = StringDef.ModifyServer + StringDef.Colon + StringDef.Success;
            return true;
        }
        return false;
    }

    private void SendGuardCfg(GameServer _server)
    {
        //发送GUARD配置文件
        if (_server != null)
        {
            if (_server.DoPlugInAction(CurrentUser.Id, GameServerControl.PlugInGuid, GameServerControl.ActionKeyReconfigureGuard))
            {
                LabelSuccess.Text = StringDef.Send + StringDef.Colon + StringDef.Success;
            }
            else
            {
                LabelOpMsg.Text = StringDef.Send + StringDef.Colon + StringDef.Failure;
            }
        }
    }

    

    protected void GameServerCountDropDownList_SelectedIndexChanged(object sender, EventArgs e)
    {
        ShowGameServer();
    }

    protected void gameServerConfigCheckBox2_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox2.Checked == false)
        {
            ButtonShowGameServerConfig2.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig2.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig2.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox3_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox3.Checked == false)
        {
            ButtonShowGameServerConfig3.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig3.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig3.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox4_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox4.Checked == false)
        {
            ButtonShowGameServerConfig4.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig4.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig4.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox5_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox5.Checked == false)
        {
            ButtonShowGameServerConfig5.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig5.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig5.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox6_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox6.Checked == false)
        {
            ButtonShowGameServerConfig6.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig6.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig6.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox7_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox7.Checked == false)
        {
            ButtonShowGameServerConfig7.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig7.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig7.Enabled = false;
        }
    }

    protected void gameServerConfigCheckBox8_OnCheckedChanged(object sender, EventArgs e)
    {
        if (gameServerConfigCheckBox8.Checked == false)
        {
            ButtonShowGameServerConfig8.Enabled = true;
        }
        else
        {
            ButtonShowGameServerConfig8.BackColor = System.Drawing.Color.Empty;
            ButtonShowGameServerConfig8.Enabled = false;
        }
    }
}
