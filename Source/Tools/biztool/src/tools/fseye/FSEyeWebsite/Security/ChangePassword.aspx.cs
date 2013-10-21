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

public partial class Security_ChangePassword : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        LabelMsg.Text = string.Empty;
    }

    protected void LinkButtonModify_Click(object sender, EventArgs e)
    {
        try
        {
            FSEye.Security.User user = TheAdminServer.SecurityManager.GetUser(CurrentUser.Id);
            if (user == null) Response.Redirect(WebConfig.PageLogin);

            string oldPassword = TextBoxOldPassword.Text;
            if (oldPassword.Length == 0)
            {
                LabelMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.OldPassword);
                return;
            }

            //检查原密码是否正确
            if (!TheAdminServer.SecurityManager.GetStoragePassword(user.UserName, oldPassword).Equals(user.Password))
            {
                LabelMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.MsgPasswordNotCorrect;
                return;
            }

            string newPassword = TextBoxNewPassword.Text.Trim();
            if (newPassword.Length == 0)
            {
                LabelMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.NewPassword);
                return;
            }

            string newPasswordConfirm = TextBoxNewPasswordConfirm.Text.Trim();
            if (!newPassword.Equals(newPasswordConfirm))
            {
                LabelMsg.Text = StringDef.PasswordConfirmFail;
                return;
            }

            user.Password = AdminServer.TheInstance.SecurityManager.GetStoragePassword(user.UserName, newPassword);
            AdminServer.TheInstance.SecurityManager.SetUser(user);
            LabelMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.ChangePassword;
        }
        catch (Exception ex)
        {
            LabelMsg.Text = ex.Message;
        }
    }
}