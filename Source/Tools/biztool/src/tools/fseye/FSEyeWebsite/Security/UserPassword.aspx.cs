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

public partial class Security_UserPassword : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        //权限检查
        //if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityUserPassword, OpType.READ, Session))
        //{
        //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        //}

        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamUserName] != null)
                TextBoxInputUserName.Text = Request.Params[WebConfig.ParamUserName] as string;
            else
                TextBoxInputUserName.Text = CurrentUser.UserName;
        }

        LabelOpMsg.Text = string.Empty;
    }

    protected void LinkButtonSetPassword_Click(object sender, EventArgs e)
    {
        string userName = TextBoxInputUserName.Text.Trim();

        if (userName == string.Empty)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.UserName);
            return;
        }

        FSEye.Security.User user = TheAdminServer.SecurityManager.GetUser(userName);
        if(user==null)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgNotExist,StringDef.UserName);
            return;
        }

        //权限检查
        if (user != null && !WebUtil.CheckPrivilege(user.SecurityObject, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        string newPassword = TextBoxInputPassword.Text.Trim();
        if(newPassword==string.Empty)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone,StringDef.Password);
            return;
        }
        string newPasswordConfirm = TextBoxInputPasswordConfirm.Text.Trim();
        if(newPassword.Equals(newPasswordConfirm))
            user.Password = TheAdminServer.SecurityManager.GetStoragePassword(userName,newPassword);
        TheAdminServer.SecurityManager.SetUser(user);
        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SetPassword;
    }
}