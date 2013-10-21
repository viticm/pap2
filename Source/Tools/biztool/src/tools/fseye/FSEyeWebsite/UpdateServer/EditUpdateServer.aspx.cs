//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : UpdateManagement
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 更新管理--修改/查看更新服务器页面
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
using Resources;
using FSEye;
using FSEye.Security;

public partial class UpDateManagement_EditUpdateServer : BasePage
{
    int serverId;
    FTPServer server;
    protected void Page_Load(object sender, EventArgs e)
    {
        //需要此页面有Read权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerEditUpdateServer, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!Page.IsPostBack)
        {
            if (Request.Params["serverId"] != null)
            {
                serverId = int.Parse(Request.Params["serverId"]);
                server = TheAdminServer.FTPClient.GetFTPServer(serverId);
                LabelTitle.Text += " : [" + server.ID + "] " + server.Literal;
            }
            if (!IsPostBack)
            {
                LoadState();
            }
        }

        LabelOpMsg.Visible = false;
    }

    private void LoadState()
    {
        if (server != null)
        {
            TextBoxServerIpAddress.Text = server.Address;
            TextBoxServerPort.Text = server.Port.ToString();
            TextBoxServerUserName.Text = server.UserName;
            TextBoxServerComment.Text = server.Literal;
        }
    }

    protected void Save_Click(object sender, EventArgs e)
    {
        //要编辑更新服务器必须对此页面有写权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerEditUpdateServer, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = "编辑服务器失败： " + StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }

        try
        {
            serverId = int.Parse(Request.Params["serverId"]);
            server = TheAdminServer.FTPClient.GetFTPServer(serverId);
            server.Address = TextBoxServerIpAddress.Text;
            server.Port = int.Parse(TextBoxServerPort.Text);
            server.UserName = TextBoxServerUserName.Text;
            if ((TextBoxServerPassword.Enabled == true) && (TextBoxServerPassword.Text.Length > 0))
                server.Password = TextBoxServerPassword.Text;
            server.Literal = TextBoxServerComment.Text;
            if (TheAdminServer.FTPClient.EditUpdateServer(server))
            {
                ShowMessage(StringDef.OperationSucceed, MessageType.Success);
                Response.Redirect("UpdateServerConfig.aspx");
            }
            else
            {
                throw new Exception();
            }
        }
        catch (Exception)
        {
            ShowMessage(StringDef.OperationFail, MessageType.Failure);
        }
    }
    protected void CheckBoxAnonymous_CheckedChanged(object sender, EventArgs e)
    {
        if (CheckBoxAnonymous.Checked)
        {
            TextBoxServerUserName.Text = "Anonymous";
            TextBoxServerUserName.Enabled = false;
        }
        else
        {
            TextBoxServerUserName.Text = "";
            TextBoxServerUserName.Enabled = true;
        }
    }

    protected void CheckBoxChangePassword_CheckedChanged(object sender, EventArgs e)
    {
        TextBoxServerPassword.Enabled = CheckBoxChangePassword.Checked;
        TextBoxPasswordConfirm.Enabled = CheckBoxChangePassword.Checked;
        RequiredFieldValidator2.Enabled = CheckBoxChangePassword.Checked;
        RequiredFieldValidator5.Enabled = CheckBoxChangePassword.Checked;        
    }
}
