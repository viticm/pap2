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
using System.Text;
using Resources;
using FSEye.Security;
using FSEye;

public partial class Security_ListUser : BasePage
{
    DataTable Store;
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityListUser, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (ViewState["Store"] != null) Store = ViewState["Store"] as DataTable;

        //CreateTableUser();
        if (!IsPostBack)
        {
            CreateDataGridUser();
        }

        LabelOpMsg.Text = string.Empty;
	}

    private void CreateCheckBoxListGroup()
    {
        if (DataGridUser.EditItemIndex != -1)
        {
            CheckBoxList groupList = DataGridUser.Items[DataGridUser.EditItemIndex].Cells[6].FindControl("CheckBoxListGroup") as CheckBoxList;
            FSEye.Security.User user = TheAdminServer.SecurityManager.GetUser((int)Store.Rows[DataGridUser.EditItemIndex].ItemArray[0]);
            if (groupList != null)
            {
                Group[] groups = TheAdminServer.SecurityManager.GetAllGroups();
                if (groups != null && groups.Length != 0)
                    foreach (Group group in groups)
                    {
                        ListItem item = new ListItem(group.SecurityObject.Name, group.SecurityObject.Id.ToString());
                        if(user!=null)
                        {
                            foreach (int groupId in user.Groups)
                            {
                                if (groupId == group.SecurityObject.Id) item.Selected = true;
                            }
                        }
                        
                        groupList.Items.Add(item);
                    }
            }
        }
    }

    private ICollection CreateDataSource(User[] users)
    {
        if (users != null && users.Length != 0)
        {
            try
            {
                DataTable dataTable = new DataTable();
                DataRow dataRow;

                dataTable.Columns.Add(new DataColumn("ObjectId", typeof(Int32)));
                dataTable.Columns.Add(new DataColumn("UserName", typeof(String)));
                dataTable.Columns.Add(new DataColumn("RealName", typeof(String)));
                dataTable.Columns.Add(new DataColumn("Comment", typeof(String)));
                dataTable.Columns.Add(new DataColumn("Group", typeof(String)));

                foreach (User user in users)
                {
                    dataRow = dataTable.NewRow();
                    dataRow[0] = user.SecurityObject.Id;
                    dataRow[1] = user.UserName;
                    dataRow[2] = user.RealName;
                    dataRow[3] = user.SecurityObject.Comment;
                    StringBuilder userGroupsInfo = new StringBuilder();
                    if (user.Groups.Length != 0)
                    {
                        foreach (int groupId in user.Groups)
                        {
                            Group group = TheAdminServer.SecurityManager.GetGroup(groupId);
                            if (group != null)
                            {
                                //userGroupsInfo.Append(group.SecurityObject.Name);
                                userGroupsInfo.AppendFormat("<a href='EditGroup.aspx?groupId={0}'>{1}</a>", group.SecurityObject.Id,group.SecurityObject.Name);
                                userGroupsInfo.Append(' ');
                            }
                        }
                        userGroupsInfo.Remove(userGroupsInfo.Length - 1, 1);
                    }
                    else
                    {
                        userGroupsInfo.Append(StringDef.NotAvailable);
                    }
                    dataRow[4] = userGroupsInfo.ToString();

                    dataTable.Rows.Add(dataRow);
                }
                ViewState["Store"] = dataTable;
                return new DataView(dataTable);
            }
            catch (Exception)
            {
                return null;
            }
        }
        return null;
    }

    private void CreateDataGridUser()
    {
        DataGridUser.DataSource = CreateDataSource(AdminServer.TheInstance.SecurityManager.GetAllUsers());
        DataGridUser.DataBind();
    }

    //protected void DataGridUser_Delete(object sender, DataGridCommandEventArgs e)
    //{
    //    string userName = e.Item.Cells[4].Text;
                
    //    FSEye.Security.User user = AdminServer.TheInstance.SecurityManager.GetUser(userName);

    //    //需要Write权限
    //    if(user!=null)
    //    {
    //        if (!WebUtil.CheckPrivilege(user.SecurityObject, OpType.WRITE, Session))
    //        {
    //            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //        }

    //        AdminServer.TheInstance.SecurityManager.DeleteUser(user.UserName);
    //    }
        
    //    CreateDataGridUser();
    //}

    //protected void DataGridUser_Edit(object sender, DataGridCommandEventArgs e)
    //{
    //    DataGridUser.EditItemIndex = e.Item.ItemIndex;
    //    DataGridUser.DataSource = new DataView(Store);
    //    DataGridUser.DataBind();
    //    //生成组的CheckBoxList
    //    CreateCheckBoxListGroup();
    //}

    //protected void DataGridUser_Cancel(object sender, DataGridCommandEventArgs e)
    //{
    //    DataGridUser.EditItemIndex = -1;
    //    DataGridUser.DataSource = new DataView(Store);
    //    DataGridUser.DataBind();
    //}

    //protected void DataGridUser_Update(object sender, DataGridCommandEventArgs e)
    //{
    //    User user = TheAdminServer.SecurityManager.GetUser((int)Store.Rows[DataGridUser.EditItemIndex].ItemArray[0]);
    //    if (user != null)
    //    {
    //        //权限检查
    //        if (!WebUtil.CheckPrivilege(user.SecurityObject, OpType.WRITE, Session))
    //        {
    //            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //        }

    //        TextBox textBoxRealName = e.Item.Cells[5].Controls[0] as TextBox;
    //        if(textBoxRealName!=null)
    //        {
    //            string newRealName = textBoxRealName.Text.Trim();
    //            if (newRealName == string.Empty)
    //            {
    //                return;
    //            }
    //            user.RealName = newRealName;
    //        }
    //        TextBox textBoxComment = e.Item.Cells[6].Controls[0] as TextBox;
    //        if(textBoxComment!=null)
    //        {
    //            string newComment = textBoxComment.Text.Trim();
    //            if (newComment == string.Empty)
    //            {
    //                return;
    //            }
    //            user.SecurityObject.Comment = newComment;
    //        }

    //        TheAdminServer.SecurityManager.SetUser(user);

    //        //添加用户到组
    //        CheckBoxList groupList = e.Item.Cells[7].FindControl("CheckBoxListGroup") as CheckBoxList;
    //        if (groupList != null)
    //        {
    //            foreach (ListItem item in groupList.Items)
    //            {
    //                if (item.Selected)
    //                {
    //                    TheAdminServer.SecurityManager.AddUserToGroup(user.SecurityObject.Id, int.Parse(item.Value));
    //                }
    //                else
    //                {
    //                    TheAdminServer.SecurityManager.RemoveUserFromGroup(user.SecurityObject.Id, int.Parse(item.Value));
    //                }
    //            }
    //        }
    //    }

    //    DataGridUser.EditItemIndex = -1;
    //    CreateDataGridUser();

    //}

    protected void LinkButton_Click(object sender, EventArgs e)
    {
        DataGridCommandEventArgs dataGridEvent = e as DataGridCommandEventArgs;
        LinkButton lb = sender as LinkButton;
        
        if(lb!=null)
        {
            switch (lb.CommandName)
            {
                case "Delete":
                    DataGridItem dataGridItem = lb.Parent.Parent as DataGridItem;
                    if(dataGridItem!=null)
                    {
                        string userName = dataGridItem.Cells[1].Text;

                        FSEye.Security.User user = AdminServer.TheInstance.SecurityManager.GetUser(userName);

                        //需要Write权限
                        if (user != null)
                        {
                            if (!WebUtil.CheckPrivilege(user.SecurityObject, OpType.WRITE, Session))
                            {
                                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                            }

                            AdminServer.TheInstance.SecurityManager.DeleteUser(user.UserName);
                        }

                        CreateDataGridUser();
                    }
                    break;
            }
        }
    }

    protected void DataGridUser_ItemDataBound(object sender, DataGridItemEventArgs e)
    {
        if (e.Item.Cells[0].Controls.Count != 0)
        {
            Control control = e.Item.Cells[0].FindControl("LinkButtonDelete");
            LinkButton lb = control as LinkButton;
            if (lb != null)
                lb.Attributes.Add("onclick", 
                    string.Format("return msgConfirm('{0}');",string.Format(StringDef.MsgDeleteConfirm,StringDef.User)));
        }
    }
}
