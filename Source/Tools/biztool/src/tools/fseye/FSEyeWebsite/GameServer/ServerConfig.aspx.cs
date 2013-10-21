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
using FSEye;
using FSEye.Security;
using FSEye.PlugIn;
using FSEye.DllWrap;
using System.IO;
public partial class GameServer_ServerConfig : System.Web.UI.Page
{
    string serverType = "";
    protected void Page_Load(object sender, EventArgs e)
    {
        serverType = Request.Params[WebConfig.ParamServerType];
        string strScript = "\n";
        if (!IsClientScriptBlockRegistered("clientScript"))
        {
            strScript = "<script>\n";
            strScript += "function CloseWin(){\n";
            strScript += "window.close();\n";
            strScript += "}\n";
            strScript += "</script>\n";
            RegisterClientScriptBlock("clientScript", strScript);
        }
        closewin.Attributes.Add("OnClick", "CloseWin()");



        if (!Page.IsPostBack)
        {
            string servercConfigFileName = WebConfig.WebsiteRootPath + WebConfig.ConfigTemplateDir + GetServerConfigFileName(serverType);
            if (serverType == "Bishop" && (string)Session["Bishop"] != null)
            {
                ServerConfig.Text = (string)Session["Bishop"];
            }
            else if (serverType == "Goddess" && (string)Session["Goddess"] != null)
            {
                ServerConfig.Text = (string)Session["Goddess"];
            }
            else if (serverType == "GameCenter" && (string)Session["GameCenter"] != null)
            {
                ServerConfig.Text = (string)Session["GameCenter"];
            }
            
            else if (serverType == "GameServer" && (string)Session["GameServer"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer"];
            }
            else if (serverType == "GameServer2" && (string)Session["GameServer2"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer2"];
            }
            else if (serverType == "GameServer3" && (string)Session["GameServer3"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer3"];
            }
            else if (serverType == "GameServer4" && (string)Session["GameServer4"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer4"];
            }
            else if (serverType == "GameServer5" && (string)Session["GameServer5"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer5"];
            }
            else if (serverType == "GameServer6" && (string)Session["GameServer6"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer6"];
            }
            else if (serverType == "GameServer7" && (string)Session["GameServer7"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer7"];
            }
            else if (serverType == "GameServer8" && (string)Session["GameServer8"] != null)
            {
                ServerConfig.Text = (string)Session["GameServer8"];
            }
            else if (serverType == "LogServer" && (string)Session["LogServer"] != null)
            {
                ServerConfig.Text = (string)Session["LogServer"];
            }

            else
            {
                ServerConfig.getServerConfigTextBox(servercConfigFileName);
            }

            ServerConfig.Text = ";Config Created Automatically by JXSJEYES\n\n" + ServerConfig.Text;

        }

    }

    private string GetServerConfigFileName(String severtype)
    {

        string serverConfigFileName = "";
        if (severtype == "Bishop")
        {
            serverConfigFileName = ServerType2Module.Gateway;
        }
        else if (severtype == "Goddess")
        {
            serverConfigFileName = ServerType2Module.RoleServer;
        }
        else if (severtype == "GameCenter")
        {
            serverConfigFileName = ServerType2Module.GameCenter;
        }
        else if ((severtype == "GameServer") || (severtype == "GameServer2") || (severtype == "GameServer3") || (severtype == "GameServer4")
                || (severtype == "GameServer5") || (severtype == "GameServer6") || (severtype == "GameServer7") || (severtype == "GameServer8"))
        {
            serverConfigFileName = ServerType2Module.GameServer;
        }
        else if (severtype == "LogServer")
        {
            serverConfigFileName = ServerType2Module.LogServer;
        }

        return serverConfigFileName;
    }



    protected void ButtonSaveFile_Click(object sender, EventArgs e)
    {

        if (serverType == "Bishop")
        {
            Session["Bishop"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "Goddess")
        {
            Session["Goddess"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameCenter")
        {
            Session["GameCenter"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }

        else if (serverType == "GameServer")
        {
            Session["GameServer"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer2")
        {
            Session["GameServer2"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer3")
        {
            Session["GameServer3"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer4")
        {
            Session["GameServer4"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer5")
        {
            Session["GameServer5"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer6")
        {
            Session["GameServer6"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer7")
        {
            Session["GameServer7"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "GameServer8")
        {
            Session["GameServer8"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }
        else if (serverType == "LogServer")
        {
            Session["LogServer"] = ServerConfig.Text;
            LabelSuccess.Text = StringDef.Save + ":" + StringDef.Success;
            LabelSuccess.Visible = true;
        }

        else
        {
            LabelOpMsg.Text = StringDef.Save + ":" + StringDef.Failure;
        }


    }


}
