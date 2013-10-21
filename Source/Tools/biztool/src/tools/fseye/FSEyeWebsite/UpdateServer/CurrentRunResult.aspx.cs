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

public partial class UpdateServer_CurrentRunResult : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!Page.IsPostBack)
        {
            if (Request.Params["SerialNum"] != null)
                HyperLinkDetail.NavigateUrl = "../GameServer/ServerOperationHistory.aspx?SerialNum=" + Request.Params["SerialNum"].ToString();

            TableHeaderRow header = new TableHeaderRow();
            TableHeaderCell[] head = new TableHeaderCell[2];
            for (int i = 0; i <= 1; i++) head[i] = new TableHeaderCell();
            head[0].Width = new Unit(10f, UnitType.Percentage);
            head[0].Text = StringDef.Name;
            head[1].Width = new Unit(30f, UnitType.Percentage);
            head[1].Text = StringDef.Message;

            header.Cells.AddRange(head);
            ResultList.Rows.Add(header);

            if (Session["ActionResultList"] == null)
            {
                TableRow row = new TableRow();
                TableCell[] cell = new TableCell[2];
                for (int i = 0; i <= 1; i++) cell[i] = new TableCell(); ;
                cell[0].Text = "";
                cell[1].Text = "请查看日志";
                row.Cells.AddRange(cell);
                ResultList.Rows.Add(row);
            }
            else
            {
                ShowRunResult(Session["ActionResultList"] as IList);
            }
        }
    }

    private void ShowRunResult(IList Result)
    {
        foreach (string[] strResult in Result)
        {            
            TableRow row = new TableRow();
            TableCell[] cell = new TableCell[2];
            for (int i = 0; i <= 1; i++) cell[i] = new TableCell();
            ServerGroup group = TheAdminServer.GameServerManager.GetGameServerGroup(int.Parse(strResult[0].ToString()));
            
            cell[0].Text = GetGroupFullName(group,group.Name);
            
                cell[1].Text = strResult[1];
                if (strResult[3].ToString() == "0")
                {
                    cell[1].BackColor = System.Drawing.Color.Red;
                }
                row.Cells.AddRange(cell);
                ResultList.Rows.Add(row);
            
        }
    }

    private string GetGroupFullName(ServerGroup group,string groupPath)
    {
        FSEyeObject groupParent = group.SecurityObject.Parent;
        while (groupParent.Parent != null )        
        {
            ServerGroup parentGroup = TheAdminServer.GameServerManager.GetGameServerGroup(groupParent.Id);
            if (parentGroup == null) break;
            groupPath = parentGroup.Name + "/" + groupPath;
            groupParent = parentGroup.SecurityObject.Parent;
        }

        return groupPath;
    }
}
