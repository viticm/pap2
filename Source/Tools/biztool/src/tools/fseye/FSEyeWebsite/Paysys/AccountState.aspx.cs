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
using FSEye.PaySysLib;
using FSEye.Security;

public partial class Paysys_AccountState : BasePage
{
    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionPaysysAccountState, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
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
        AccountState state = TheAdminServer.PaySysAgent.GetAccountState(accountName);
        ShowState(state);
        LabelResult.Text = string.Format("[{0}] {1}", accountName, StringDef.AccountState);        
        TableResult.Visible = true;
    }

    private void ShowState(AccountState state)
    {
        TableRow row = new TableRow();
        TableHeaderCell cell = new TableHeaderCell();
        cell.Width = new Unit(30, UnitType.Percentage);
        cell.Text = StringDef.AccountState;
        row.Cells.Add(cell);

        TableCell cell1 = new TableCell();
        cell1.Text = state.ToString();
        row.Cells.Add(cell1);

        TableResult.Rows.Add(row);
    }
}
