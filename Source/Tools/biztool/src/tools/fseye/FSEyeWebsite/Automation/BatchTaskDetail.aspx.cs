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
using System.Collections.Generic;
using Resources;
using FSEye.Security;

public partial class Automation_BatchTaskDetail : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationBatchTaskDetail, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

		try
		{
            int taskId = int.Parse(Request.Params[WebConfig.ParamTaskId]);
            BatchTask task = TheAdminServer.BatchTaskManager.GetBatchTask(taskId);
            if (task != null)
            {
                SetTask(task);
            }
            else
            {
                throw new Exception();
            }
		}
		catch (Exception)
		{
			Response.Redirect("BatchTask.aspx", true);
		}
	}

	void SetTask(BatchTask task)
	{
		if (task != null)
		{
            LiteralName.Text = task.SecurityObject.Name;
			LiteralDesc.Text = task.SecurityObject.Comment;
			if (task.StartTime != DateTime.MinValue)
			{
				LiteralBeginTime.Text = task.StartTime.ToString();
			}
			else
			{
				LiteralBeginTime.Text = StringDef.NotStarted;
			}
			if (task.StartTime != DateTime.MinValue)
			{
				if (task.EndTime != DateTime.MinValue)
				{
					LiteralEscapedTime.Text = ((TimeSpan)(task.EndTime.Subtract(task.StartTime))).ToString();
				}
				else
				{
					LiteralEscapedTime.Text = ((TimeSpan)(DateTime.Now.Subtract(task.StartTime))).ToString();
				}
			}
			else
			{
				LiteralEscapedTime.Text = StringDef.NotStarted;
			}
			if (task.EndTime != DateTime.MinValue)
			{
				LiteralEndTime.Text = task.EndTime.ToString();
			}
			else
			{
				LiteralEndTime.Text = StringDef.NotFinished;
			}

			TableServerList.Rows.Clear();

			int[] serverIds = task.Servers;
			if (serverIds != null)
			{
				TableHeaderRow headerRow = new TableHeaderRow();
				TableHeaderCell headerCell = null;

				headerCell = new TableHeaderCell();
				headerCell.Text = StringDef.GameServer;
				headerRow.Cells.Add(headerCell);

				headerCell = new TableHeaderCell();
				headerCell.Text = StringDef.State;
				headerRow.Cells.Add(headerCell);

				TableServerList.Rows.Add(headerRow);

				for (int i = 0; i < serverIds.Length; i++)
				{
					GameServer server = TheAdminServer.GameServerManager.GetGameServer(serverIds[i]);
					if (server != null)
					{
						TableRow row = new TableRow();
						TableCell cell = null;

						cell = new TableCell();
						cell.Text = server.Name;
						row.Cells.Add(cell);

						cell = new TableCell();
						cell.Text = task.GetUnitState(i).ToString();
						row.Cells.Add(cell);

						TableServerList.Rows.Add(row);
					}
				}
			}
		}
	}
}
