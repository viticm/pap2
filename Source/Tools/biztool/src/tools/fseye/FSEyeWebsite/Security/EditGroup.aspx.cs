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
using FSEye.Security;

public partial class Security_EditGroup : BasePage
{
	Group _group;

	public int GroupId
	{
		get { return _group.SecurityObject.Id; }
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityEditGroup, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params[WebConfig.ParamGroupId] != null)
		{
			try
			{
				int groupId = int.Parse(Request.Params[WebConfig.ParamGroupId]);
				_group = AdminServer.TheInstance.SecurityManager.GetGroup(groupId);
                //修改需要Write权限
                //if (_group != null && !WebUtil.CheckPrivilege(_group.SecurityObject, OpType.WRITE, Session))
                //{
                //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                //}

				HyperLinkEditPrivilege.NavigateUrl = string.Format("EditPrivilege.aspx?objectId={0}", groupId);
				HyperLinkAddUserToGroup.NavigateUrl = string.Format("UserGroup.aspx?groupId={0}", groupId);
			}
			catch (Exception)
			{
				//Response.Redirect("ListGroup.aspx");
			}
		}
		else
		{
			Response.Redirect("ListGroup.aspx");
		}

		if (!IsPostBack)
		{
			SetGroupInfo(_group);
			SetUserInGroupInfo(_group);
		}
	}

	protected void LinkButtonModify_Click(object sender, EventArgs e)
	{
		string groupComment = TextBoxComment.Text;
		string originalComment = _group.SecurityObject.Comment;		

		if (_group != null && !WebUtil.CheckPrivilege(_group.SecurityObject, OpType.WRITE, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		try
		{
			_group.SecurityObject.Comment = groupComment;
			AdminServer.TheInstance.SecurityManager.SetGroup(_group);;
			ShowMessage("Modify group \"" + _group.SecurityObject.Name + "\" success.", MessageType.Success);
		}
		catch (Exception ex)
		{
			_group.SecurityObject.Comment = originalComment;
			ShowMessage(ex.Message, MessageType.Failure);
		}
	}

	private void SetGroupInfo(Group group)
	{
		TextBoxGroupName.Text = group.SecurityObject.Name;
		TextBoxComment.Text = group.SecurityObject.Comment;
	}

	private void SetUserInGroupInfo(Group group)
	{
		User[] users = AdminServer.TheInstance.SecurityManager.GetUserInGroup(group.SecurityObject.Id);
		if (users != null)
		{
			for (int i = 0; i < users.Length; i++)
			{
				User user = users[i];

				TableRow row = new TableRow();
				TableCell cell = new TableCell();
				HyperLink linkRemoveGroup = new HyperLink();
				linkRemoveGroup.NavigateUrl = "UserGroup.aspx?userId=" + user.SecurityObject.Id + "&groupId=" + _group.SecurityObject.Id;
				linkRemoveGroup.Text = StringDef.Remove;
				linkRemoveGroup.SkinID = "SmallButton";
				cell.Controls.Add(linkRemoveGroup);
				row.Cells.Add(cell);
				cell = new TableCell();
				cell.Text = (i + 1).ToString();
				row.Cells.Add(cell);
				cell = new TableCell();
				cell.Text = user.SecurityObject.Id.ToString();
				row.Cells.Add(cell);
				cell = new TableCell();
				HyperLink linkEditUser = new HyperLink();
				linkEditUser.NavigateUrl = "EditUser.aspx?userId=" + user.SecurityObject.Id;
				linkEditUser.Text = user.SecurityObject.Name;
				linkEditUser.SkinID = "PlainText";
				cell.Controls.Add(linkEditUser);
				row.Cells.Add(cell);
				cell = new TableCell();
				cell.Text = user.SecurityObject.Comment;
				row.Cells.Add(cell);

				TableUserInGroup.Rows.Add(row);
			}
		}
	}
}
