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
using FSEye;
using FSEye.Security;

public partial class Common_SystemLogout : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        UserInfo info = CurrentUser;
        if (info != null)
        {
            LabelInfo.Text = info.ToString();
//            Response.Write(info);
        }

		ButtonLogout.Focus();
    }

    protected void ButtonLogout_Click(object sender, EventArgs e)
    {
        if (CurrentUser != null)
        {
			try
			{
				AdminServer.TheInstance.SecurityManager.Logout(CurrentUser.Id);
				CurrentUser = null;
				Session.Abandon();
			}
			catch (Exception)
			{
				Response.Redirect(WebConfig.PageLogin);
			}
        }

        Response.Redirect(WebConfig.PageHome);
    }
}
