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
using FSEye.DAL;

public partial class Security_AddUser : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityAddUser, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		ClearMessage();
	}

	protected void LinkButtonAdd_Click(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(TheAdminServer.SecurityManager.UsersSecurityObject, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
		string userName = TextBoxUserName.Text;
		string comment = TextBoxComment.Text;
		string realName = TextBoxRealName.Text;
		string password = TextBoxPassword.Text;

		try
		{
			AdminServer.TheInstance.SecurityManager.AddUser(userName, password, realName, comment);

			Session[WebConfig.SessionOpResult] = new OperationResult("Add user \"" + userName + "\" success.", MessageType.Success);

			User user = AdminServer.TheInstance.SecurityManager.GetUser(userName);
            //if (user != null)
            //{
            //    Response.Redirect("EditUser.aspx?userId=" + user.SecurityObject.Id);
            //}
            //else
            //{
				Response.Redirect("ListUser.aspx");
            //}
		}
		catch (Exception ex)
		{
			ShowMessage(ex.Message, MessageType.Failure);
		}
	}
}
