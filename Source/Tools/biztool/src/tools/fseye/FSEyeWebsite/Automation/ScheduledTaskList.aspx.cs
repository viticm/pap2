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

public partial class Automation_ScheduledTaskList : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationScheduledTaskList, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			ListScheduledTask();
		}

		RefreshButtonSwitchSchedulerOn();
        LabelOpMsg.Text = string.Empty;
	}

	void ListScheduledTask()
	{
		ListItemCollection list = ListBoxScheduledTask.Items;
		list.Clear();

		IList scheduledTaskList = TheAdminServer.ScheduledTaskManager.ScheduledTaskList;
		for (int i = 0; i < scheduledTaskList.Count; i++)
		{
			ScheduledTaskUnit unit = scheduledTaskList[i] as ScheduledTaskUnit;
			string itemText = unit.Task.ToString();
            if (!unit.Task.Enabled)
            {
                itemText += " [" + StringDef.Disable + "]";
            }
            ListItem item = new ListItem(itemText, unit.Task.SecurityObject.Id.ToString());
            //string color = unit.Task.Enabled ? "color:Green;" : "color:Red;";
            //item.Attributes.Add("style", color);
			list.Add(item);
		}

		ShowScheduledTaskCount();
	}

	void ShowScheduledTaskCount()
	{
		LabelSchedulerSummary.Text = string.Format(StringDef.TotalScheduledTaskCount, TheAdminServer.ScheduledTaskManager.ScheduledTaskList.Count);
	}

	protected void ButtonSwitchSchedulerOn_Click(object sender, EventArgs e)
	{
        //要开启和关闭计划任务模块需要在ScheduledTaskManager上有执行权限
        if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
		TheAdminServer.ScheduledTaskManager.On = !TheAdminServer.ScheduledTaskManager.On;
		RefreshButtonSwitchSchedulerOn();
	}

	void RefreshButtonSwitchSchedulerOn()
	{
		if (TheAdminServer.ScheduledTaskManager.On)
		{
            LabelSchedulerOn.Text = StringDef.ScheduledTaskIsRunning;
			LabelSchedulerOn.CssClass = CssConfig.MessageSuccess;
			ButtonSwitchSchedulerOn.Text = StringDef.StopAllScheduledTask;
		}
		else
		{
			LabelSchedulerOn.Text = StringDef.ScheduledTaskAlreadyClosed;
			LabelSchedulerOn.CssClass = CssConfig.MessageFailure;
			ButtonSwitchSchedulerOn.Text = StringDef.StartAllScheduledTask;
		}
	}

	protected void LinkButtonModify_Click(object sender, EventArgs e)
	{
		int taskId = 0;
		try
		{
			taskId = int.Parse(ListBoxScheduledTask.SelectedValue);
		}
		catch (Exception)
		{
		}

		if (taskId > 0)
		{
            ScheduledTaskUnit taskUnit = TheAdminServer.ScheduledTaskManager.Get(taskId);
			if (taskUnit != null)
			{
				Session[WebConfig.SessionScheduledTask] = taskUnit;
				Response.Redirect("AddScheduledTask.aspx?" + WebConfig.ParamOperation + "=" + WebConfig.OpTypeModify);
			}			
		}
	}

	protected void LinkButtonEnable_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxScheduledTask.Items)
		{
			if (item.Selected)
			{
				int taskId = int.Parse(item.Value);
                //要启用自动化作业需要在ScheduledTaskManager上有执行权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
				TheAdminServer.ScheduledTaskManager.Enable(taskId, true);
			}
		}

		ListScheduledTask();
        RefreshTaskDescription();
	}

	protected void LinkButtonDisable_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxScheduledTask.Items)
		{
			if (item.Selected)
			{
				int taskId = int.Parse(item.Value);
                //要禁用自动化作业需要在ScheduledTaskManager上有执行权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
				TheAdminServer.ScheduledTaskManager.Enable(taskId, false);
			}
		}
		ListScheduledTask();
        RefreshTaskDescription();
	}

	protected void LinkButtonDel_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxScheduledTask.Items)
		{

			if (item.Selected)
			{
				int taskId = int.Parse(item.Value);
                //要删除自动化作业需要在ScheduledTaskManager上有Write的权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.WRITE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
				TheAdminServer.ScheduledTaskManager.Delete(taskId);
			}
		}

		ListScheduledTask();
        RefreshTaskDescription();
	}

    protected void RefreshTaskDescription(object sender, EventArgs e)
    {
        RefreshTaskDescription();
    }
    private void RefreshTaskDescription()
    {
        if (ListBoxScheduledTask.SelectedIndex != -1)
        {
            PannelTaskDes.Visible = true;
            ScheduledTaskUnit unit = TheAdminServer.ScheduledTaskManager.ScheduledTaskList[ListBoxScheduledTask.SelectedIndex] as ScheduledTaskUnit;
            LabelTaskState1.Text = unit.Task.Enabled ? StringDef.Enable : StringDef.Disable;
            string color = unit.Task.Enabled ? "color:Green;" : "color:Red;";
            LabelTaskState1.Attributes.Add("style", color);            
            LiteralTask1.Text = unit.Task.Type.ToString();
            LiteralAutomation1.Text = unit.Task.Automation.Name;
            LiteralTaskNextExecuteTime1.Text = unit.Task.NextExecuteTime == DateTime.MinValue ? StringDef.NotAvailable : unit.Task.NextExecuteTime.ToString();
        }
        else PannelTaskDes.Visible = false;
    }

    protected void LinkButtonExport_Click(object sender, EventArgs e)
    {
        string fileName = SystemConfig.Current.AdminServerUploadFileRootPath + WebConfig.FileTaskList;

        ArrayList taskToExport = new ArrayList();
        foreach (ListItem item in ListBoxScheduledTask.Items)
        {
            if (item.Selected)
            {
                int taskId = int.Parse(item.Value);
                ScheduledTaskUnit taskUnit = TheAdminServer.ScheduledTaskManager.Get(taskId);
                if (taskUnit != null) taskToExport.Add(taskUnit);
            }
        }

        if (taskToExport != null && taskToExport.Count != 0)
        {
            if (WebUtil.ExportScheduledTask(fileName, taskToExport))
                LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + string.Format(StringDef.MsgAutomationExport, fileName, StringDef.ScheduledTaskList);
            else
                LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + string.Format(StringDef.MsgAutomationExport, fileName, StringDef.ScheduledTaskList);
        }
    }

    protected void LinkButtonImport_Click(object sender, EventArgs e)
    {
        string fileName = SystemConfig.Current.AdminServerUploadFileRootPath + WebConfig.FileTaskList;
        if (WebUtil.ImportScheduledTask(fileName))
        {
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + string.Format(StringDef.MsgAutomationImport, fileName, StringDef.ScheduledTaskList);
        }
        else
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + string.Format(StringDef.MsgAutomationImport, fileName, StringDef.ScheduledTaskList);
        }
        ListScheduledTask();
    }
}
