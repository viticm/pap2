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

public partial class Security_AddGroup : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityAddGroup, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		ClearMessage();
	}

	protected void LinkButtonAdd_Click(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(TheAdminServer.SecurityManager.GroupsSecurityObject, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
		string groupName = TextBoxGroupName.Text;
		string comment = TextBoxComment.Text;

		try
		{
			AdminServer.TheInstance.SecurityManager.AddGroup(groupName, comment);

			Session[WebConfig.SessionOpResult] = new OperationResult("Add group \"" + groupName + "\" success.", MessageType.Success);

			Group group = AdminServer.TheInstance.SecurityManager.GetGroup(groupName);			
			if (group != null)
			{
				Response.Redirect("EditGroup.aspx?groupId=" + group.SecurityObject.Id);
			}
			else
			{
				Response.Redirect("ListGroup.aspx");
			}			
		}
		catch (Exception ex)
		{
			ShowMessage(ex.Message, MessageType.Failure);
		}
	}
}
