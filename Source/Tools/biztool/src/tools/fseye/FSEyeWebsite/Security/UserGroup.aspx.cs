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

public partial class Security_UserGroup : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityUserGroup, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
		//if (!WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.UsersSecurityObject, OpType.READ, Session)
		//    || !WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.GroupsSecurityObject, OpType.READ, Session))
		//{
		//    Response.Redirect(WebConfig.PageNotEnoughPrivilege);
		//}

		ClearMessage();

		if (!IsPostBack)
		{
			int userId = 0;
			int groupId = 0;
			try
			{
				if (Request.Params[WebConfig.ParamUserId] != null)
				{
					userId = int.Parse(Request.Params[WebConfig.ParamUserId]);
				}
				if (Request.Params[WebConfig.ParamGroupId] != null)
				{
					groupId = int.Parse(Request.Params[WebConfig.ParamGroupId]);
				}
			}
			catch (FormatException)
			{
			}

			User[] users = AdminServer.TheInstance.SecurityManager.GetAllUsers();
			foreach (User user in users)
			{
				ListItem item = new ListItem(user.UserName, user.SecurityObject.Id.ToString());
				if (user.SecurityObject.Id == userId)
				{
					item.Selected = true;
				}
				DropDownListUser.Items.Add(item);
			}

			Group[] groups = AdminServer.TheInstance.SecurityManager.GetAllGroups();
			foreach(Group group in groups)
			{
				ListItem item = new ListItem(group.SecurityObject.Name, group.SecurityObject.Id.ToString());
				if (group.SecurityObject.Id == groupId)
				{
					item.Selected = true;
				}
				DropDownListGroup.Items.Add(item);
			}
		}
	}

	protected void LinkButtonReturn_Click(object sender, EventArgs e)
	{
		try
		{
			int userId = 0;
			int groupId = 0;
			if (Request.Params[WebConfig.ParamUserId] != null)
			{
				userId = int.Parse(Request.Params[WebConfig.ParamUserId]);
			}
			if (Request.Params[WebConfig.ParamGroupId] != null)
			{
				groupId = int.Parse(Request.Params[WebConfig.ParamGroupId]);
			}

			if (userId > 0)
			{
				Response.Redirect("EditUser.aspx?userId=" + userId);
			}
			if (groupId > 0)
			{
				Response.Redirect("EditGroup.aspx?groupId=" + groupId);
			}
		}
		catch (FormatException)
		{
			Response.Redirect("Security.aspx");
		}
	}

	protected void LinkButtonAdd_Click(object sender, EventArgs e)
	{		
		try
		{
			int userId = int.Parse(DropDownListUser.Text);
			int groupId = int.Parse(DropDownListGroup.Text);

			if (WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Get(userId), OpType.WRITE, Session)
				&& WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Get(groupId), OpType.WRITE, Session))
			{
				AdminServer.TheInstance.SecurityManager.AddUserToGroup(userId, groupId);
				LabelMessage.Text = string.Format("Add user[{0}] to group[{1}]: {2}", userId, groupId, StringDef.Success);
			}
			else
			{
				LabelMessage.Text = StringDef.NotEnoughPrivilege;
			}
		}
		catch (Exception ex)
		{
			LabelMessage.Text = string.Format("{0}: {1}", MessageType.Failure, ex.Message);
		}
	}

	protected void LinkButtonRemove_Click(object sender, EventArgs e)
	{
		try
		{
			int userId = int.Parse(DropDownListUser.Text);
			int groupId = int.Parse(DropDownListGroup.Text);

			if (WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Get(userId), OpType.WRITE, Session)
				&& WebUtil.CheckPrivilege(AdminServer.TheInstance.SecurityManager.Get(groupId), OpType.WRITE, Session))
			{
				AdminServer.TheInstance.SecurityManager.RemoveUserFromGroup(userId, groupId);
				LabelMessage.Text = string.Format("Remove user[{0}] from group[{1}]: {2}", userId, groupId, StringDef.Success);
			}
			else
			{
				LabelMessage.Text = StringDef.NotEnoughPrivilege;
			}
		}
		catch (Exception ex)
		{
			LabelMessage.Text = string.Format("{0}: {1}", MessageType.Failure, ex.Message);
		}
	}
}
