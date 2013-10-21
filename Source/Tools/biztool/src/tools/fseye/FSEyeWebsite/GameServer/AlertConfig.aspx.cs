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

public partial class GameServer_AlertConfig : BasePage
{
	public const string OperationRemoveAlertCondition = "removeAlertCondition";

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAlertConfig, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			TextBoxCheckPeriod.Text = TheAdminServer.GameServerMonitor.CheckPeriod.ToString();
		}

		if (Request.Params[WebConfig.ParamOperation] == OperationRemoveAlertCondition)
		{
			try
			{
				int index = int.Parse(Request.Params[WebConfig.ParamIndex]);
				RemoveAlertCondition(index);

				Response.Redirect("AlertConfig.aspx");
			}
			catch(Exception)
			{
			}
		}

		CreateTableAlertCondition();
        LabelOpMsg.Text = string.Empty;
	}

	protected void LinkButtonApply_Click(object sender, EventArgs e)
	{
		try
		{
			long period = long.Parse(TextBoxCheckPeriod.Text);
            //写权限
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAlertConfig, OpType.WRITE, Session))
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return;
            }
			TheAdminServer.GameServerMonitor.CheckPeriod = period;
            LabelOpMsg.Text = StringDef.OperationSucceed;
		}
		catch(Exception)
		{
			//TODO 提示错误
		}
	}

	private void CreateTableAlertCondition()
	{
		IList alertConditions = TheAdminServer.GameServerMonitor.AlertConditions;

		for (int i = 0; i < alertConditions.Count; i++)
		{
			AlertCondition condition = alertConditions[i] as AlertCondition;
			
			TableRow row = new TableRow();
			TableCell cell = new TableCell();
			LinkButton linkButtonRemove = new LinkButton();
            linkButtonRemove.Attributes.Add(WebConfig.ParamIndex, i.ToString());
			linkButtonRemove.SkinID = "PlainText";
			//linkButtonRemove.NavigateUrl = "AlertConfig.aspx?" + WebConfig.ParamOperation + "=" + OperationRemoveAlertCondition + "&" + WebConfig.ParamIndex + "=" + i;
            linkButtonRemove.Click += LinkButtonRemove_Click;
			linkButtonRemove.Text = StringDef.Remove;
            linkButtonRemove.ID = "LinkButtonRemove"+i.ToString();
			cell.Controls.Add(linkButtonRemove);
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.Text = (i + 1).ToString();
			row.Cells.Add(cell);
			cell = new TableCell();
			cell.Text = condition.ToString();
			row.Cells.Add(cell);

			TableAlertCondition.Rows.Add(row);
		}
	}

	public void RemoveAlertCondition(int index)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAlertConfig, OpType.WRITE, Session))
        {
            LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
            return;
        }
		TheAdminServer.GameServerMonitor.AlertConditions.RemoveAt(index);
	}

    protected void LinkButtonRemove_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            string indexText = lb.Attributes[WebConfig.ParamIndex];
            if (indexText != null && indexText.Length != 0)
            {
                int index = int.Parse(indexText);
                RemoveAlertCondition(index);
            }
        }
    }
}
