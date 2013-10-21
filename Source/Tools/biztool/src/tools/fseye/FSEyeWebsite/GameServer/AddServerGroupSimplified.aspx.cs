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
using System.Xml.XPath;
using Resources;
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;
using FSEye.DllWrap;

public partial class GameServer_AddServerGroupSimplified : BasePage
{
    GameServer _server = null;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAddServerGroupSimplified, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            TextBoxFSEyeIp.Text = SystemConfig.Current.AdminServerListenIP;
            int defaultPort = SystemConfig.Current.AdminServerListenPort;
            TextBoxFSEyePort.Text = Convert.ToString(defaultPort);
            
            ServerGroupDropDownList.ListEmptyServerGroups();
            FtpDropDownList.ListFtpByType(FTPServer.FTPServerType.ServerUpdate);
        }

        CreateConfigParametersTableByXml();
    }

    private void CreateConfigParametersTableByXml()
    {
        ArrayList parametersArrayList = new ArrayList();

        XPathNavigator nav;
        XPathDocument docNav;

        docNav = new XPathDocument(WebConfig.WebsiteRootPath + "AddServerGroupConfigParameters.xml");
        nav = docNav.CreateNavigator();
        nav.MoveToRoot();
        nav.MoveToFirstChild();
        nav.MoveToFirstChild();

        do
        {
            if (nav.NodeType == XPathNodeType.Element)
            {
                if (nav.HasChildren)
                {
                    nav.MoveToFirstChild();

                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[3];
                    for (int i = 0; i < 3; i++)
                        cell[i] = new TableCell();

                    cell[0].Width = new Unit(25f, UnitType.Percentage);
                    cell[0].Style.Value = "text-align: center;font-weight: bold;color: #FFFFFF;background-color: #5099B3;height: 20px;border-bottom: solid 1px #808080;border-right: solid 1px #808080;";
                    cell[1].Width = new Unit(20f, UnitType.Percentage);
                    cell[2].Width = new Unit(80f, UnitType.Percentage);

                    string textBoxName = String.Empty;
                    
                    TextBox textBox = new TextBox();

                    do
                    {
                        if (nav.LocalName == "Name")
                        {
                            cell[0].Text = nav.Value;
                            textBoxName = nav.Value.Remove(0, 1);
                            textBoxName = textBoxName.Remove(textBoxName.Length - 1, 1);
                            parametersArrayList.Add(textBoxName);
                            textBox.ID = textBoxName;
                        }
                        else if (nav.LocalName == "Note")
                        {
                            cell[2].Text = nav.Value;
                        }
                        else if (nav.LocalName == "DefaultValue")
                        {
                            textBox.Text = nav.Value;
                        }
                    }
                    while (nav.MoveToNext());
                   
                    RequiredFieldValidator validator = new RequiredFieldValidator();
                    validator.ControlToValidate = textBox.ID;
                    validator.ID = validator.ControlToValidate + "Validator";
                    validator.Display = ValidatorDisplay.None;
                    validator.SetFocusOnError = true;
                    validator.ErrorMessage = StringDef.RequiredFieldValidatorErrorMsg;
                    AjaxControlToolkit.ValidatorCalloutExtender extender = new AjaxControlToolkit.ValidatorCalloutExtender();
                    extender.TargetControlID = validator.ID;
                    cell[1].Controls.Add(textBox);
                    cell[1].Controls.Add(validator);
                    cell[1].Controls.Add(extender);

                    row.Cells.AddRange(cell);
                    ConfigParametersTable.Rows.Add(row);
                }
            }

            nav.MoveToParent();
        }
        while (nav.MoveToNext());

        Session["ConfigParameters"] = parametersArrayList;
    }

    protected void LinkButtonSave_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }
        
        LabelSuccess.Text = "";
        LabelOpMsg.Text = "";
        ArrayList servers = new ArrayList();

        //获得服务器组
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        
        //选中的服务器组为空的情况
        if (group == null)
        {
            LabelOpMsg.Text = "没有选中的服务器组，添加终止";
            LabelOpMsg.Visible = true;

            return;
        }

        //此服务器组中已有服务器，添加中止
        //（只允许对一个空组做本页面添加一整组的操作）
        if (group.List.Count != 0)
        {
            LabelOpMsg.Text = "本服务器组中已有其他服务器，添加中止（只允许对空服务器组做此添加操作）";
            LabelOpMsg.Visible = true;

            return;
        }
        
        //已有其他server使用输入的IP，添加终止
        string groupIp = TextBoxGroupIpAddress.Text.Trim();
        foreach (GameServer server in TheAdminServer.GameServerManager.Servers)
        {
            if (server.IpAddress == groupIp)
            {
                LabelOpMsg.Text = "已有服务器使用此IP: " + server.Name;
                LabelOpMsg.Visible = true;

                return;
            }
        }

        try
        {
            string strName = "";
            string strComment = "";
            string strDir = "";
            string serverConfigFileName = "";
            string serverConfigFile = "";
            GameServer svr = null;

            //添加bishop
            strName = group.Name + "_网关服务器";
            strComment = strName;
            strDir = "../bishop/";
            serverConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + ServerType2Module.Gateway;
            ServerConfigTextBox.getServerConfigTextBox(serverConfigFileName);
            serverConfigFile = GetServerConfigFile(ServerConfigTextBox.Text);
            TheAdminServer.GameServerManager.AddGameServer(strName, strComment, groupIp, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.bishop, FtpDropDownList.SelectFtp);
            svr = getGameServerByName(strName);
            servers.Add(svr);


            //添加goddess
            strName = group.Name + "_角色管理服务器";
            strComment = strName;
            strDir = "../goddess/";
            serverConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + ServerType2Module.RoleServer;
            ServerConfigTextBox.getServerConfigTextBox(serverConfigFileName);
            serverConfigFile = GetServerConfigFile(ServerConfigTextBox.Text);
            TheAdminServer.GameServerManager.AddGameServer(strName, strComment, groupIp, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.goddess, FtpDropDownList.SelectFtp);
            svr = getGameServerByName(strName);
            servers.Add(svr);


            //添加gamecenter
            strName = group.Name + "_中心服务器";
            strComment = strName;
            strDir = "../gamecenter/";
            serverConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + ServerType2Module.GameCenter;
            ServerConfigTextBox.getServerConfigTextBox(serverConfigFileName);
            serverConfigFile = GetServerConfigFile(ServerConfigTextBox.Text);
            TheAdminServer.GameServerManager.AddGameServer(strName, strComment, groupIp, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.gamecenter, FtpDropDownList.SelectFtp);
            svr = getGameServerByName(strName);
            servers.Add(svr);


            //添加logserver
            strName = group.Name + "_日志服务器";
            strComment = strName;
            strDir = "../logserver/";
            serverConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + ServerType2Module.LogServer;
            ServerConfigTextBox.getServerConfigTextBox(serverConfigFileName);
            serverConfigFile = GetServerConfigFile(ServerConfigTextBox.Text);
            TheAdminServer.GameServerManager.AddGameServer(strName, strComment, groupIp, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.logserver, FtpDropDownList.SelectFtp);
            svr = getGameServerByName(strName);
            servers.Add(svr);

            //添加gameservers
            for (int i = 1; i <= 7; i++)
            {
                strName = group.Name + "_游戏服务器0" + i.ToString();
                strComment = strName;
                strDir = "../gameserver0" + i.ToString() + "/";

                serverConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + ServerType2Module.GameServer;
                ServerConfigTextBox.getServerConfigTextBox(serverConfigFileName);
                serverConfigFile = GetServerConfigFile(ServerConfigTextBox.Text, i);

                TheAdminServer.GameServerManager.AddGameServer(strName, strComment, groupIp, strDir, group, GameServer.ConfigGuardState.Configuring, serverConfigFile, GameServer.ServerType.gameserver, FtpDropDownList.SelectFtp);
                GameServer server = getGameServerByName(strName);
                servers.Add(server);
            }



            if (servers.Count == 0)
            {
                //LabelOpMsg.Text = StringDef.AddServer + StringDef.Failure + StringDef.Colon + StringDef.ConfigFile + StringDef.NotExist;

                LabelOpMsg.Text = StringDef.AddServer + StringDef.Failure + StringDef.Colon + "未指定任何要添加的服务器";

                LabelOpMsg.Visible = true;
            }
            else
            {
                foreach (GameServer server in servers)
                {
                    LordConfig lordConfig = GuardCfg(server.IpAddress);
                    server.LordConfig.ServerCount = lordConfig.ServerCount;
                    server.LordConfig.ServerIDs = lordConfig.ServerIDs;
                    server.LordConfig.BindIp = server.IpAddress;

                    //只对GameCenter的LordConfig添加GMCIp和GMCAccount的信息
                    if (server.Type == GameServer.ServerType.gamecenter)
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
                    TheAdminServer.GameServerManager.ModifyGameServer(server, server.Name, server.Comment, server.IpAddress, server.GameDir, server.Group, lordConfig, GameServer.ConfigGuardState.Configuring, server.ConfigContent, server.Type, FtpDropDownList.SelectFtp);
                    SendGuardCfg(svr);
                }
                LabelSuccess.Text = StringDef.AddServer + StringDef.Colon + StringDef.Ok;
                LabelSuccess.Visible = true;

                LinkButtonEdit.Enabled = false;

                SaveArgumentsAsDefaultValues();
            }
        }
        catch (Exception ex)
        {
            foreach (GameServer svr in servers)
            {
                TheAdminServer.GameServerManager.DeleteGameServer(svr);
            }

            Util.DebugLog(ex.ToString());

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

    private string GetServerConfigFile(string config)
    {
        string resultConfig = config;
        ArrayList parametersArrayList = Session["ConfigParameters"] as ArrayList;
        TextBox textbox =null;        

        foreach (string parameter in parametersArrayList)
        {
            string toBeReplacedParameter = "#" + parameter + "#";
            textbox = ConfigParametersTable.FindControl(parameter) as TextBox;
            string argument = textbox.Text;
            resultConfig = resultConfig.Replace(toBeReplacedParameter, argument);
        }

        return resultConfig;
    }

    private string GetServerConfigFile(string config, int i)
    {
        string resultConfig = config;
        ArrayList parametersArrayList = Session["ConfigParameters"] as ArrayList;
        TextBox textbox = null;

        foreach (string parameter in parametersArrayList)
        {
            string toBeReplacedParameter = "#" + parameter + "#";
            textbox = UpdatePanel1.FindControl(parameter) as TextBox;
            string argument = textbox.Text;
            resultConfig = resultConfig.Replace(toBeReplacedParameter, argument);
        }

        resultConfig = resultConfig.Replace("#SVR_ID#", i.ToString());

        return resultConfig;
    }

    private void SaveArgumentsAsDefaultValues()
    {
        XmlDocument xmlDoc = new XmlDocument();
        xmlDoc.Load(WebConfig.WebsiteRootPath + "AddServerGroupConfigParameters.xml");
        XmlNode root = xmlDoc.DocumentElement;

        ArrayList parametersArrayList = Session["ConfigParameters"] as ArrayList;
        TextBox textbox = null;

        foreach (string parameter in parametersArrayList)
        {
            string toBeReplacedParameter = "#" + parameter + "#";
            textbox = UpdatePanel1.FindControl(parameter) as TextBox;
            string argument = textbox.Text;

            XmlNode parameterNode = root.SelectSingleNode("Parameter[Name='" + toBeReplacedParameter + "']");

            XmlNode defaultValueNode = parameterNode.LastChild;
            defaultValueNode.InnerText = argument;
        }

        xmlDoc.Save(WebConfig.WebsiteRootPath + "AddServerGroupConfigParameters.xml");       
    }
}