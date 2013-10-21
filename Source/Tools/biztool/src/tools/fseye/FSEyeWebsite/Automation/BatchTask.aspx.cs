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

public partial class BatchTask_BatchTask : BasePage
{
	CheckBox[] _selectedCheckBox;

	protected void Page_Load(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationBatchTask, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

		ListTask();
	}

	void ListTask()
	{
		TableTask.Rows.Clear();

		BatchTaskManager manager = TheAdminServer.BatchTaskManager;
		IList<BatchTask> taskList = manager.TaskList;
		if (taskList != null && taskList.Count > 0)
		{
			LabelNoTask.Visible = false;
			TableTask.Visible = true;
			ButtonDelete.Enabled = true;
			ButtonExecute.Enabled = true;
			ButtonPause.Enabled = true;
			ButtonContinue.Enabled = true;

			_selectedCheckBox = new CheckBox[taskList.Count];

			TableRow headerRow = new TableRow();
			TableHeaderCell headerCell = null;

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.Selected;
			headerRow.Cells.Add(headerCell);

            headerCell = new TableHeaderCell();
            headerCell.Text = StringDef.Name;
            headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.Desc;
			headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.State;
			headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.StartTime;
			headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.EscapedTime;
			headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.EndTime;
			headerRow.Cells.Add(headerCell);

			headerCell = new TableHeaderCell();
			headerCell.Text = StringDef.Operation;
			headerCell.Width = 200;
			headerRow.Cells.Add(headerCell);

			TableTask.Rows.Add(headerRow);

			for (int i = 0; i < taskList.Count; i++)
			{
				BatchTask task = taskList[i];
				TableRow row = new TableRow();
				TableCell cell = null;

				cell = new TableCell();
				_selectedCheckBox[i] = new CheckBox();
                _selectedCheckBox[i].InputAttributes["id"] = task.ID.ToString();
				cell.Controls.Add(_selectedCheckBox[i]);
				row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = task.SecurityObject.Name;
                row.Cells.Add(cell);

				cell = new TableCell();
				cell.Text = task.SecurityObject.Comment;
				row.Cells.Add(cell);

				cell = new TableCell();
				cell.Text = task.TaskState.ToString();
				row.Cells.Add(cell);

				cell = new TableCell();
				if (task.StartTime != DateTime.MinValue)
				{
					cell.Text = task.StartTime.ToString();
				}
				else
				{
					cell.Text = StringDef.NotStarted;
				}
				row.Cells.Add(cell);

				cell = new TableCell();
				if (task.StartTime != DateTime.MinValue)
				{
					if (task.EndTime != DateTime.MinValue)
					{
						cell.Text = ((TimeSpan)(task.EndTime.Subtract(task.StartTime))).ToString();
					}
					else
					{
						cell.Text = ((TimeSpan)(DateTime.Now.Subtract(task.StartTime))).ToString();
					}
				}
				else
				{
					cell.Text = StringDef.NotStarted;
				}
				row.Cells.Add(cell);

				cell = new TableCell();
				if (task.EndTime != DateTime.MinValue)
				{
					cell.Text = task.EndTime.ToString();
				}
				else
				{
					cell.Text = StringDef.NotFinished;
				}
				row.Cells.Add(cell);

				cell = new TableCell();
				HyperLink viewDetailLink = new HyperLink();
				viewDetailLink.SkinID = "PlainText";
				viewDetailLink.Text = StringDef.ViewDetail;
                viewDetailLink.NavigateUrl = string.Format("BatchTaskDetail.aspx?{0}={1}", WebConfig.ParamTaskId, task.ID.ToString());
				cell.Controls.Add(viewDetailLink);

				HyperLink modifyLink = new HyperLink();
				modifyLink.SkinID = "PlainText";
				modifyLink.Enabled = (task.TaskState != BatchTask.State.Doing);
				modifyLink.Text = StringDef.Modify;
                modifyLink.NavigateUrl = string.Format("EditBatchTask.aspx?op=modify&{0}={1}", WebConfig.ParamTaskId, task.ID.ToString());
				cell.Controls.Add(modifyLink);
				row.Cells.Add(cell);

				TableTask.Rows.Add(row);
			}
		}
		else
		{
			LabelNoTask.Visible = true;
			TableTask.Visible = false;
			ButtonDelete.Enabled = false;
			ButtonExecute.Enabled = false;
			ButtonPause.Enabled = false;
			ButtonContinue.Enabled = false;
		}
	}

	protected void ButtonExecute_Click(object sender, EventArgs e)
	{
		try
		{
            if (!WebUtil.CheckPrivilege(TheAdminServer.BatchTaskManager.SecurityObject, OpType.EXECUTE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

			foreach (CheckBox checkBox in _selectedCheckBox)
			{
				if (checkBox.Checked)
				{
                    int taskId = int.Parse(checkBox.InputAttributes["id"]);
                    BatchTask task = TheAdminServer.BatchTaskManager.GetBatchTask(taskId);
					task.Reset();
					task.Start();
				}
			}
		}
		catch (Exception)
		{
		}
	}

	protected void ButtonPause_Click(object sender, EventArgs e)
	{
		try
		{
            if (!WebUtil.CheckPrivilege(TheAdminServer.BatchTaskManager.SecurityObject, OpType.EXECUTE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

			foreach (CheckBox checkBox in _selectedCheckBox)
			{
				if (checkBox.Checked)
				{
                    int taskId = int.Parse(checkBox.InputAttributes["id"]);
                    BatchTask task = TheAdminServer.BatchTaskManager.GetBatchTask(taskId);
					task.Pause();
				}
			}
		}
		catch (Exception)
		{
		}
	}

	protected void ButtonContinue_Click(object sender, EventArgs e)
	{
		try
		{
            if (!WebUtil.CheckPrivilege(TheAdminServer.BatchTaskManager.SecurityObject, OpType.EXECUTE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

			foreach (CheckBox checkBox in _selectedCheckBox)
			{
				if (checkBox.Checked)
				{
                    int taskId = int.Parse(checkBox.InputAttributes["id"]);
                    BatchTask task = TheAdminServer.BatchTaskManager.GetBatchTask(taskId);
					task.Continue();

				}
			}
		}
		catch (Exception)
		{
		}
	}

	protected void ButtonDelete_Click(object sender, EventArgs e)
	{
		try
		{
            if (!WebUtil.CheckPrivilege(TheAdminServer.BatchTaskManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

			foreach (CheckBox checkBox in _selectedCheckBox)
			{
				if (checkBox.Checked)
				{
                    int taskId = int.Parse(checkBox.InputAttributes["id"]);
                    TheAdminServer.BatchTaskManager.DeleteTask(taskId);
				}
			}
            ListTask();
		}
		catch (Exception)
		{
		}
	}
}