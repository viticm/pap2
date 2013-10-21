//////////////////////////////////////////////////////////////////////
//
//      Kingsoft Blaze Game Studio. Copyright (C) 2006
//
//      Created_datetime : 2007-4
//      File_base        : UpdateManagement
//      File_ext         : cs
//      Author           : 姚遥
//      Description      : 更新管理--更新服务器页面,主要是更新服务器
//                         列表,提供有添加,删除和修改的功能.
//
//////////////////////////////////////////////////////////////////////

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

public partial class UpDateManagement_UpdateServerConfig : BasePage
{
    DataTable Store;
    protected void Page_Load(object sender, EventArgs e)
    {
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUpdateServerConfig, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

        if (!IsPostBack)
        {
            //ListFTPServer();
            ListUpdateServer();
        }
        if (ViewState["Store"] != null) Store = ViewState["Store"] as DataTable;
        ClearMessage();
        LabelOpMsg.Visible = false;
    }

    //private void ListFTPServer()
    //{
    //    FTPServerCheckBoxList.Items.Clear();
    //    if (TheAdminServer.FTPClient.FTPServerList.Count == 0)
    //    {
    //        NoUpdateServer.Visible = true;
    //        return;
    //    }
    //    else
    //    {
    //        NoUpdateServer.Visible = false;
    //        foreach (FTPServer server in TheAdminServer.FTPClient.FTPServerList)
    //        {
    //            string href = @"<a href = 'EditUpdateServer.aspx?serverId=" + server.ID + "' style='text-decoration:none;' >" + server.Literal + "</a>";
    //            ListItem item = new ListItem("[" + server.ID + "]" + href, server.ID.ToString());
    //            FTPServerCheckBoxList.Items.Add(item);
    //        }
    //        FTPServerCheckBoxList.ToolTip = StringDef.ClickToViewState;
    //    }
    //}

    //private void SetAllServers(bool select)
    //{
    //    foreach (ListItem item in FTPServerCheckBoxList.Items)
    //    {
    //        item.Selected = select;
    //    }
    //}

    //protected void ButtonSelectAllServers_Click(object sender, EventArgs e)
    //{
    //    SetAllServers(true);
    //}

    //protected void ButtonSelectNoServer_Click(object sender, EventArgs e)
    //{
    //    SetAllServers(false);
    //}

    //protected void ButtonModify_Click(object sender, EventArgs e)
    //{
    //    if (FTPServerCheckBoxList.SelectedItem != null)
    //    {
    //        Response.Redirect("EditUpdateServer.aspx?serverId=" + FTPServerCheckBoxList.SelectedItem.Value);
    //    }
    //    else
    //    {
    //        ShowMessage(StringDef.ChooseUpdateServer, MessageType.Attention);
    //    }
    //}

    //protected void ButtonDelete_Click(object sender, EventArgs e)
    //{
    //    try
    //    {
    //        bool hasTarget = false;
    //        foreach (ListItem item in FTPServerCheckBoxList.Items)
    //        {
    //            int serverId = int.Parse(item.Value);
    //            if (item.Selected)
    //            {
    //                //删除需要对该服务器有写的权限
    //                if (!WebUtil.CheckPrivilege(TheAdminServer.SecurityManager.Get(serverId), OpType.WRITE, Session))
    //                {
    //                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //                }
    //                TheAdminServer.FTPClient.DeleteUpdateServer(serverId);
    //                hasTarget = true;
    //            }
    //        }
    //        if (!hasTarget) throw new Exception();
    //        ShowMessage(StringDef.OperationSucceed, MessageType.Success);
    //        ListFTPServer();
    //    }
    //    catch (Exception)
    //    {
    //        ShowMessage(StringDef.OperationFail, MessageType.Failure);
    //    }
    //}

    private void ListUpdateServer()
    {
        DataGridUpdateServer.DataSource = CreateDataSource(TheAdminServer.FTPClient.FTPServerList);
        DataGridUpdateServer.DataBind();
    }

    private ICollection CreateDataSource(IList serverList)
    {
        if (serverList.Count == 0)
        {
            NoUpdateServer.Visible = true;
            return null;
        }
        else
        {
            NoUpdateServer.Visible = false;
            DataTable dataTable = new DataTable();
            DataRow dataRow;

            dataTable.Columns.Add(new DataColumn("Name", typeof(String)));
            dataTable.Columns.Add(new DataColumn("IP", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Port", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("UserName", typeof(String)));
            dataTable.Columns.Add(new DataColumn("Password", typeof(String)));
            dataTable.Columns.Add(new DataColumn("ServerId", typeof(Int32)));
            dataTable.Columns.Add(new DataColumn("ServerType", typeof(String)));

            foreach (FTPServer server in serverList)
            {
                dataRow = dataTable.NewRow();
                dataRow[0] = server.Literal;
                dataRow[1] = server.Address;
                dataRow[2] = server.Port;
                dataRow[3] = server.UserName;
                dataRow[4] = "********"; // server.Password;
                dataRow[5] = server.ID;                

                switch (server.FtpServerType)
                {
                    case FTPServer.FTPServerType.ServerUpdate:
                        dataRow[6] = "服务端更新FTP";
                        break;
                    case FTPServer.FTPServerType.ClientUpdate:
                        dataRow[6] = "客户端更新FTP";
                        break;
                    case FTPServer.FTPServerType.ClientServerListFile:
                        dataRow[6] = "客户端ServerList更新FTP";
                        break;

                    default:
                        dataRow[6] = "未指定服务器组";
                        break;
                }

                dataTable.Rows.Add(dataRow);

                //string href = @"<a href = 'editupdateserver.aspx?serverid=" + server.id + "' style='text-decoration:none;' >" + server.literal + "</a>";
                //listitem item = new listitem("[" + server.id + "]" + href, server.id.tostring());
                //ftpservercheckboxlist.items.add(item);
            }

            dataTable.DefaultView.Sort = "ServerType, ServerId";

            ViewState["Store"] = dataTable;
            return dataTable.DefaultView;
        }
    }

    protected void DataGridUpdateServer_Delete(object sender, DataGridCommandEventArgs e)
    {
        //删除更新服务器需要在此页面上有Write的权限
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUpdateServerConfig, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = "删除服务器失败： " + StringDef.NotEnoughPrivilege;
            LabelOpMsg.Visible = true;
            return;
        }
        
        int serverId = int.Parse(e.Item.Cells[9].Text);
        TheAdminServer.FTPClient.DeleteUpdateServer(serverId);
        ListUpdateServer();
    }

    protected void DataGridUpdateServer_ItemDataBound(object sender, DataGridItemEventArgs e)
    {
        if (e.Item.Cells[0].Controls.Count != 0)
        {
            LinkButton lb = e.Item.Cells[0].Controls[0] as LinkButton;
            if (lb != null)
                lb.Attributes.Add("onclick",
                    string.Format("return deleteConfirm('{0}');", string.Format(StringDef.MsgDeleteConfirm, StringDef.UpdateServer)));
        }
    }
}