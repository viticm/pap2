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

public partial class Security_Who : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityWho, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
		//if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.SecurityObject, OpType.READ, Session)
		//    || !WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.UsersSecurityObject, OpType.READ, Session))
		//{
		//    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		//}

		ListLoginUsers();
	}

	void ListLoginUsers()
	{
		User[] users = TheAdminServer.SecurityManager.GetAllLoginUsers();
		if (users != null)
		{
			for (int i = 0; i < users.Length; i++)
			{
				User user = users[i];

				TableRow row = new TableRow();				
				
				TableCell cell = new TableCell();
				cell.Text = user.UserName;
				row.Cells.Add(cell);
				
				cell = new TableCell();
				cell.Text = user.ClientAddress;
				row.Cells.Add(cell);

				cell = new TableCell();
				cell.Text = user.LoginTime.ToString();
				row.Cells.Add(cell);
				
				TableLoginUser.Rows.Add(row);
			}
		}
	}
}
