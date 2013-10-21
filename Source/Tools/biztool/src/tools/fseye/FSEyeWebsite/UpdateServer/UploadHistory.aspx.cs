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
using Resources;
using FSEye.Security;

public partial class UpdateServer_UploadHistory : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionUpdateServerUploadHistory, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
            ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
            if (group != null) ServerDropDownList.CreateServerList(group);
        }

    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void buttonDispenseHistory_Click(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(TheAdminServer.GameServerManager.SecurityObject, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }   
        
        Timer1.Enabled = true;
    }

    protected void Timer1_Tick1(object sender, EventArgs e)
    {
        ListServerFiles();
    }

    protected void ListServerFiles()
    {
        if (ServerDropDownList.ServerCount > 0 && ServerDropDownList.SelectedServerId > 0)
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            TableHeaderRow header = new TableHeaderRow();
            TableHeaderCell[] head = new TableHeaderCell[2];
            for (int i = 0; i <= 1; i++) head[i] = new TableHeaderCell();
            head[0].Width = new Unit(10f, UnitType.Percentage);
            head[0].Text = StringDef.Time;
            head[1].Width = new Unit(30f, UnitType.Percentage);
            head[1].Text = StringDef.Message;

            header.Cells.AddRange(head);
            
            ServerFileList.Rows.Add(header);

            if (server.UpdateMsg.Count == 0)
            {
                if (server.IsConnected)
                {
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[2];
                    for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                    cell[0].Text = server.Name;
                    cell[1].Text = StringDef.Running;
                    row.Cells.AddRange(cell);
                    ServerFileList.Rows.Add(row);
                }
                else
                {
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[2];
                    for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                    cell[0].Text = server.Name;
                    cell[1].Text = StringDef.NoConnectionAlert;
                    cell[1].BackColor = System.Drawing.Color.Red;
                    row.Cells.AddRange(cell);
                    ServerFileList.Rows.Add(row);
                }
            }
            else
            {
                foreach (string[] str in server.UpdateMsg)
                {
                    TableRow row = new TableRow();
                    TableCell[] cell = new TableCell[2];
                    for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                    cell[0].Text = str[4]; // server.Name;
                    cell[1].Text = str[0] + "/" + str[1] + ":" + str[3];
                    if (str[2] == "0")
                    {
                        cell[1].BackColor = System.Drawing.Color.Red;
                    }

                    row.Cells.AddRange(cell);
                    ServerFileList.Rows.Add(row);
                }
            }
        }
    }
}
