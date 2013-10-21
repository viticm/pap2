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
using Resources;

public partial class Common_SystemControl : System.Web.UI.Page
{
	User _user;

    public string GetControlMessage
    {
        get
        {
            if (AdminServer.TheInstance.IsRunning)
            {
                //关闭提示信息
                return StringDef.StopServiceMessage;
            }
            else
            {
                //开启提示信息
                return StringDef.StartServiceMessage;
            }
        }
    }

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSystemSystemControl, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		_user = WebUtil.GetUser(Session);

		if (_user == null && !WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Root, OpType.READ, Session))
		{
			ButtonServiceSwitch.Visible = false;
		}
		else
		{
			ButtonServiceSwitch.Visible = true;
			ButtonServiceSwitch.Text = AdminServer.TheInstance.IsRunning ? StringDef.StopService : StringDef.StartService;
		}
        LabelMessage.Text = string.Empty;
	}

	protected void ButtonServiceSwitch_Click(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Root, OpType.WRITE, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        try
        {
            if (AdminServer.TheInstance.IsRunning)
            {
                AdminServer.TheInstance.StopService(_user);
            }
            else
            {
                AdminServer.TheInstance.StartService(_user);
            }

            ButtonServiceSwitch.Text = AdminServer.TheInstance.IsRunning ? StringDef.StopService : StringDef.StartService;
        }
        catch (Exception ex)
        {
            LabelMessage.Text = ex.Message;
        }
	}
}
