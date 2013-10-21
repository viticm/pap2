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

public partial class Security_DeleteUser : BasePage
{
	int _userId;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityDeleteUser, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		ClearMessage();

        try
        {
            _userId = 0;
            if (Request.Params[WebConfig.ParamUserId] != null)
            {
                _userId = int.Parse(Request.Params[WebConfig.ParamUserId]);
                //if (!WebUtil.CheckPrivilege(_userId, OpType.WRITE, Session))
                //{
                //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                //}
            }
        }
        catch (FormatException)
        {
        }

		if (!IsPostBack)
		{
			User user = AdminServer.TheInstance.SecurityManager.GetUser(_userId);
			if (user != null)
			{
				TextBoxUserName.Text = user.UserName;
			}			
		}
	}

	protected void LinkButtonDelete_Click(object sender, EventArgs e)
	{
		string userName = TextBoxUserName.Text;

		try
        {
            //需要Write权限
            if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.GetUser(userName).SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }           

			AdminServer.TheInstance.SecurityManager.DeleteUser(userName);

			Session[WebConfig.SessionOpResult] = new OperationResult("Delete user \"" + userName + "\" success.", MessageType.Success);
			Response.Redirect("ListUser.aspx");
		}
		catch (Exception ex)
		{
			ShowMessage(ex.Message, MessageType.Failure);
		}
	}
}
