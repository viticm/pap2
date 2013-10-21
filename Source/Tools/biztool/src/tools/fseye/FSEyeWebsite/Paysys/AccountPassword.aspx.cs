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

public partial class Paysys_AccountPassword : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionPaysysAccountPassword, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            if (Request.Params[WebConfig.ParamAccount] != null)
            {
                TextBoxInputAccount.Text = Request.Params[WebConfig.ParamAccount];
            }

            LinkButtonSetPassword.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.SetPassword)));
            LinkButtonSavePassword.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.SavePassword)));
            LinkButtonResetPassword.Attributes.Add("onclick",
                string.Format("return msgConfirm('{0}');", string.Format(StringDef.MsgConfirm, StringDef.ResetPassword)));
        }

        LabelOpMsg.Text = string.Empty;
    }

    protected void LinkButtonSetPassword_Click(object sender, EventArgs e)
    {
        string accountName = TextBoxInputAccount.Text.Trim();
        if (accountName.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Account);
            return;
        }
        string accountPassword = TextBoxInputPassword.Text;
        if (accountPassword.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Password);
            return;
        }
        FSEye.PaySysLib.OperationResult result = TheAdminServer.PaySysAgent.SetPassword(accountName, accountPassword, false);
        LabelOpMsg.Text = result.ToString();        
    }

    protected void LinkButtonPassword_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            string accountName;
            string accountPassword;
            switch (lb.CommandName)
            {
                case "SavePass":
                    accountName = TextBoxInputAccount.Text;
                    accountPassword = TheAdminServer.PaySysAgent.GetPassword(accountName);
                    if (accountPassword == null || accountPassword.Length == 0)
                    {
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SavePassword;
                        return;
                    }
                    if (!TheAdminServer.PaysysManager.SaveAccountPassword(accountName, accountPassword))
                    {
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SavePassword;
                    }
                    else
                    {
                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SavePassword;
                    }
                    break;
                case "ResetPass":
                    accountName = TextBoxInputAccount.Text;
                    accountPassword = TheAdminServer.PaysysManager.GetSavedPassword(accountName);
                    if (accountPassword == null || accountPassword.Length == 0)
                    {
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.ResetPassword;
                        return;
                    }
                    FSEye.PaySysLib.OperationResult result = TheAdminServer.PaySysAgent.SetPassword(accountName, accountPassword, true);
                    if (result != FSEye.PaySysLib.OperationResult.Success)
                    {
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + result.ToString();
                    }
                    else
                    {
                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.ResetPassword;
                    }
                    break;
            }
        }
    }
}
