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

public partial class Security_EditUser : BasePage
{
	User _user;

	public int UserId
	{
		get { return _user.SecurityObject.Id; }
	}

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityEditUser, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params[WebConfig.ParamUserId] != null)
		{			
			try
			{
				int userId = int.Parse(Request.Params[WebConfig.ParamUserId]);
                _user = AdminServer.TheInstance.SecurityManager.GetUser(userId);
				HyperLinkEditPrivilege.NavigateUrl = "EditPrivilege.aspx?objectId=" + userId;
				HyperLinkAddUserToGroup.NavigateUrl = "UserGroup.aspx?userId=" + userId;
			}
			catch (Exception)
			{
				//Response.Redirect("ListUser.aspx");
			}
		}
		else
		{
			Response.Redirect("ListUser.aspx");
		}

		if (!IsPostBack)
		{
			SetUserBasicInfo(_user);
			SetUserInGroupInfo(_user);
            //CreateCheckBoxListGroup();
		}

        LabelOpMsg.Text = string.Empty;
	}

	private void SetUserBasicInfo(User user)
	{		
		TextBoxUserName.Text = user.UserName;
		TextBoxRealName.Text = user.RealName;
		TextBoxComment.Text = user.SecurityObject.Comment;
	}

    private void SetUserInGroupInfo(User user)
    {
        int[] groups = user.Groups;
        for (int i = 0; i < groups.Length; i++)
        {
            Group group = AdminServer.TheInstance.SecurityManager.GetGroup(groups[i]);

            TableRow row = new TableRow();
            TableCell cell = new TableCell();
            HyperLink linkRemoveGroup = new HyperLink();
            linkRemoveGroup.NavigateUrl = "UserGroup.aspx?userId=" + _user.SecurityObject.Id + "&groupId=" + group.SecurityObject.Id;
            linkRemoveGroup.Text = StringDef.Remove;
            linkRemoveGroup.SkinID = "SmallButton";
            cell.Controls.Add(linkRemoveGroup);
            row.Cells.Add(cell);
            cell = new TableCell();
            cell.Text = (i + 1).ToString();
            row.Cells.Add(cell);
            cell = new TableCell();
            cell.Text = group.SecurityObject.Id.ToString();
            row.Cells.Add(cell);
            cell = new TableCell();
            HyperLink linkEditGroup = new HyperLink();
            linkEditGroup.NavigateUrl = "EditGroup.aspx?groupId=" + group.SecurityObject.Id;
            linkEditGroup.Text = group.SecurityObject.Name;
            linkEditGroup.SkinID = "PlainText";
            cell.Controls.Add(linkEditGroup);
            row.Cells.Add(cell);
            cell = new TableCell();
            cell.Text = group.SecurityObject.Comment;
            row.Cells.Add(cell);

            TableUserInGroup.Rows.Add(row);
        }
    }

    protected void LinkButtonSave_Click(object sender, EventArgs e)
	{
        if (_user != null)
        {
            if (_user.UserName.Equals(CurrentUser.UserName, StringComparison.OrdinalIgnoreCase))
            {
                //用户自己修改自己-->需要原密码确认
                string oldPassword = TextBoxOldPassword.Text;
                if (!TheAdminServer.SecurityManager.GetStoragePassword(_user.UserName, oldPassword).Equals(_user.Password))
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.MsgPasswordNotCorrect;
                    return;
                }
            }
            else if (!WebUtil.CheckPrivilege(_user.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            string newPassword = TextBoxPassword.Text.Trim();
            string newPasswordConfirm = TextBoxPasswordConfirm.Text.Trim();
            string comment = TextBoxComment.Text;
            string realName = TextBoxRealName.Text;

            if (newPassword.Length != 0)
            {
                if (!newPassword.Equals(newPasswordConfirm))
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.PasswordConfirmFail;
                    return;
                }
            }
            try
            {
                _user.SecurityObject.Comment = comment;
                _user.RealName = realName;
                if (newPassword != null && newPassword.Length > 0)
                {
                    _user.Password = AdminServer.TheInstance.SecurityManager.GetStoragePassword(_user.UserName, newPassword);
                }
                AdminServer.TheInstance.SecurityManager.SetUser(_user);
                Response.Redirect("ListUser.aspx",true);
            }
            catch (Exception)
            {
            }
        }

        ////添加用户到组

        //foreach (ListItem item in CheckBoxListGroup.Items)
        //{
        //    if (item.Selected)
        //    {
        //        TheAdminServer.SecurityManager.AddUserToGroup(_user.SecurityObject.Id, int.Parse(item.Value));
        //        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.Group + string.Format("[{0}]", item.Text) + StringDef.NotEnoughPrivilege;
        //    }
        //    else
        //    {
        //        TheAdminServer.SecurityManager.RemoveUserFromGroup(_user.SecurityObject.Id, int.Parse(item.Value));
        //    }
        //}

        //Response.Redirect("ListUser.aspx",true);
		//ShowMessage("Modify user \"" + _user.UserName + "\" success.", MessageType.Success);        
	}

    //private void CreateCheckBoxListGroup()
    //{
    //    Group[] groups = TheAdminServer.SecurityManager.GetAllGroups();
    //    if (groups != null && groups.Length != 0)
    //        foreach (Group group in groups)
    //        {
    //            ListItem item = new ListItem(string.Format("<a href='EditGroup.aspx?groupId={0}'>{1}</a>", group.SecurityObject.Id, group.SecurityObject.Name), group.SecurityObject.Id.ToString());
    //            if (_user != null)
    //            {
    //                foreach (int groupId in _user.Groups)
    //                {
    //                    if (groupId == group.SecurityObject.Id) item.Selected = true;
    //                }
    //            }
    //            CheckBoxListGroup.Items.Add(item);
    //        }
    //}
}
