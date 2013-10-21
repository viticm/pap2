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

public partial class Security_DeleteGroup : BasePage
{
	int _groupId;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityDeleteGroup, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		ClearMessage();

		try
		{
			_groupId = 0;
			if (Request.Params[WebConfig.ParamGroupId] != null)
			{
				_groupId = int.Parse(Request.Params[WebConfig.ParamGroupId]);
				if (!WebUtil.CheckPrivilege(_groupId, OpType.WRITE, Session))
				{
					Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
				}
			}			
		}
		catch (FormatException)
		{
		}

		if (!IsPostBack)
		{			
			Group group = AdminServer.TheInstance.SecurityManager.GetGroup(_groupId);
			if (group != null)
			{
				TextBoxGroupName.Text = group.SecurityObject.Name;
			}
		}
	}

	protected void LinkButtonDelete_Click(object sender, EventArgs e)
	{
		string groupName = TextBoxGroupName.Text;

		try
		{
			AdminServer.TheInstance.SecurityManager.DeleteGroup(groupName);

			if (_groupId > 0)
			{
				Session[WebConfig.SessionOpResult] = new OperationResult("Delete group \"" + groupName + "\" success.", MessageType.Success);
				Response.Redirect("ListGroup.aspx");
			}
		}
		catch (Exception ex)
		{
			ShowMessage(ex.Message, MessageType.Failure);
		}
	}
}
