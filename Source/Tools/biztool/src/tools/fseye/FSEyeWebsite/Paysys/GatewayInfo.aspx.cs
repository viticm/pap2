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
using System.Collections.Generic;
using Resources;
using FSEye.PaySysLib;
using FSEye.Security;

public partial class Paysys_GatewayList : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionPaysysGatewayInfo, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            //TextBoxInputAccount.Attributes.Add("onmouseover", string.Format("Tip('{0}',SHADOW,true,SHADOWWIDTH, 3,BORDERCOLOR ,'Black',BGCOLOR,'FFFFE7',ABOVE,true,FOLLOWMOUSE ,false)", StringDef.MsgPaysysGatewayInfo));            
        }
        LabelOpMsg.Text = string.Empty;
    }

    protected void LinkButtonQuery_Click(object sender, EventArgs e)
    {
        string accountName = TextBoxInputAccount.Text.Trim();
        if (accountName.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Account);
            return;
        }
        ShowGatewayInfo(accountName);        
    }

    private void ShowGatewayList(IList<GatewayInfo> list)
    {
        if (list == null || list.Count == 0)
        {
            return;
        }
        TableHeaderRow rowHead = new TableHeaderRow();
        TableHeaderCell cellHead = new TableHeaderCell();
        cellHead.Text = StringDef.GatewayName;
        rowHead.Cells.Add(cellHead);

        cellHead = new TableHeaderCell();
        cellHead.Text = StringDef.ZoneName;
        rowHead.Cells.Add(cellHead);

        cellHead = new TableHeaderCell();
        cellHead.Text = StringDef.IPAddress;
        rowHead.Cells.Add(cellHead);

        TableGatewayInfo.Rows.Add(rowHead);

        foreach (GatewayInfo info in list)
        {
            TableRow row = new TableRow();

            TableCell cell = new TableCell();
            cell.Text = info.GatewayName;
            row.Cells.Add(cell);

            cell = new TableCell();
            cell.Text = info.ZoneName;
            row.Cells.Add(cell);

            cell = new TableCell();
            cell.Text = info.IPAddress;
            row.Cells.Add(cell);

            TableGatewayInfo.Rows.Add(row);
        }
        LabelResult.Text = StringDef.GatewayList;
    }

    private void ShowGatewayInfo(string accountName)
    {
        string gatewayInfo = TheAdminServer.PaySysAgent.GetGatewayByAccount(accountName);
        if (gatewayInfo == null)
        {
            LabelOpMsg.Text = StringDef.MsgAccountNotExist;
            return;
        }
        TableRow row = new TableRow();
        TableHeaderCell cellHead = new TableHeaderCell();
        cellHead.Text = StringDef.GatewayInfo;
        row.Cells.Add(cellHead);

        TableGatewayInfo.Rows.Add(row);

        row = new TableRow();
        TableCell cell = new TableCell();
        cell.Text = gatewayInfo;
        row.Cells.Add(cell);
        TableGatewayInfo.Rows.Add(row);

        LabelResult.Text = string.Format("[{0}] {1}", accountName, StringDef.GatewayInfo);
    }

    protected void LinkButtonUpdateGateway_Click(object sender, EventArgs e)
    {
        IList<GatewayInfo> list = TheAdminServer.PaySysAgent.GetGatewayList();
        if (list != null && list.Count != 0)
        {
            TheAdminServer.PaySysAgent.GatewayList = list;
            ShowGatewayList(list);
        }
        else
        {
            LabelOpMsg.Text = StringDef.OperationFail;
        }
    }
}
