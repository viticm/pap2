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

public partial class Automation_AddScheduledTask : BasePage
{
	enum PageOpType
	{
		Add,
		Modify
	}

	PageOpType _opType;

	ScheduledTaskUnit _taskUnit; 

	protected void Page_Load(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationAddScheduledTask, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

		string op = Request.Params[WebConfig.ParamOperation];
		if (op != null && op == WebConfig.OpTypeModify && Session[WebConfig.SessionScheduledTask] != null)
		{
			_opType = PageOpType.Modify;
            _taskUnit = Session[WebConfig.SessionScheduledTask] as ScheduledTaskUnit;
            
            if (_taskUnit == null) return;
            
            //修改自动化作业需要对ScheduledTaskManager有写权限
            if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
		}
		else
		{
			_opType = PageOpType.Add;

            //添加自动化作业需要对ScheduledTaskManager有写权限
            if (!WebUtil.CheckPrivilege(TheAdminServer.ScheduledTaskManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
		}
		
		if (!IsPostBack)
		{
			ListScheduledTaskType();

			if (_opType == PageOpType.Modify)
			{
				LabelOpType.Text = StringDef.EditScheduledTask;
                if (_taskUnit != null)
				{
                    AutomationEditor1.Automation = _taskUnit.Task.Automation;
                    TextBoxName.Text = _taskUnit.Task.SecurityObject.Name;
                    TextBoxComment.Text = _taskUnit.Task.SecurityObject.Comment;

                    switch (_taskUnit.Task.Type)
					{
						case ScheduledTaskType.Once:
							{
                                OnceScheduledTask task = _taskUnit.Task as OnceScheduledTask;
								TextBoxExecuteTime.Text = task.ExecuteTime.ToString();
							}
							break;
						case ScheduledTaskType.Repeat:
							{
                                RepeatScheduledTask task = _taskUnit.Task as RepeatScheduledTask;
								TextBoxStartTime.Text = task.StartTime.ToString();
								TextBoxEndTime.Text = task.EndTime.ToString();
								TextBoxInterval.Text = task.Interval.ToString();
							}
							break;
                        case ScheduledTaskType.DayOfWeek:
                            {
                                DayOfWeekScheduledTask task = _taskUnit.Task as DayOfWeekScheduledTask;
                                TextBoxDayOfWeekExecuteTime.Text = task.TimeOfDay.ToString();
                                foreach (DayOfWeek day in task.DayOfWeek)
                                    CheckBoxListDayOfWeek.Items[(int)day].Selected = true;
                            }
                            break;
					}

                    SetScheduledTaskType(_taskUnit.Task.Type);
				}
			}
			else if (_opType == PageOpType.Add)
			{
				AutomationEditor1.Automation = null;
				LabelOpType.Text = StringDef.AddScheduledTask;
			}
            LabelOnceExecuteTime.Text += "[yyyy-mm-dd hh:mm:ss]";
            LabelRepeatStartTime.Text += "[hh:mm:ss]";
            LabelRepeatEndTime.Text += "[hh:mm:ss]";
            LabelIntervalTime.Text += "[hh:mm:ss]";
            LabelDayOfWeekExecuteTime.Text += "[hh:mm:ss]";
		}
        OperateResult.Text = string.Empty;
	}

	void ListScheduledTaskType()
	{
		DropDownListScheduledTaskType.Items.Clear();
		DropDownListScheduledTaskType.Items.Add(new ListItem(StringDef.Once, ((int)ScheduledTaskType.Once).ToString()));
		DropDownListScheduledTaskType.Items.Add(new ListItem(StringDef.Repeatable, ((int)ScheduledTaskType.Repeat).ToString()));	
	    DropDownListScheduledTaskType.Items.Add(new ListItem(StringDef.DayOfWeek,((int)ScheduledTaskType.DayOfWeek).ToString()));
	}

	protected void FinishButton_Click(object sender, EventArgs e)
	{
		try
		{
			IAutomation automation = AutomationEditor1.Automation;
            if (automation != null)
            {
                string name = TextBoxName.Text.Trim();
                string comment = TextBoxComment.Text.Trim();

                foreach (ScheduledTaskUnit unit in TheAdminServer.ScheduledTaskManager.ScheduledTaskList)
                {
                    if (unit.Task.SecurityObject.Name == name)
                    {
                        //重名的判定：
                        //1.添加时与任何一个已有的计划任务名称相同
                        //2.修改时与除本身外的任何一个已有的计划任务名称相同
                        if ((_opType == PageOpType.Add) || ((_opType == PageOpType.Modify) && (unit.Task.SecurityObject.Id != _taskUnit.Task.SecurityObject.Id)))
                        {
                            OperateResult.Text = "该计划任务名称已存在" + StringDef.Colon + "计划任务ID - " + unit.Task.SecurityObject.Id.ToString();

                            return;
                        }
                    }
                }

                ScheduledTaskType type = (ScheduledTaskType)(int.Parse(DropDownListScheduledTaskType.SelectedValue));

                if (_opType == PageOpType.Add)
                {
                    IScheduledTask task = null;
                    switch (type)
                    {
                        case ScheduledTaskType.Once:
                            {
                                OnceScheduledTask onceTask = new OnceScheduledTask();
                                onceTask.Automation = automation;
                                onceTask.ExecuteTime = DateTime.Parse(TextBoxExecuteTime.Text);
                                task = onceTask;
                            }
                            break;
                        case ScheduledTaskType.Repeat:
                            {
                                RepeatScheduledTask repeatTask = new RepeatScheduledTask();
                                repeatTask.Automation = automation;
                                repeatTask.StartTime = TimeSpan.Parse(TextBoxStartTime.Text);
                                repeatTask.EndTime = TimeSpan.Parse(TextBoxEndTime.Text);
                                repeatTask.Interval = TimeSpan.Parse(TextBoxInterval.Text);
                                task = repeatTask;
                            }
                            break;
                        case ScheduledTaskType.DayOfWeek:
                            {
                                DayOfWeekScheduledTask dayOfWeekTask = new DayOfWeekScheduledTask();
                                dayOfWeekTask.Automation = automation;
                                dayOfWeekTask.TimeOfDay = TimeSpan.Parse(TextBoxDayOfWeekExecuteTime.Text);
                                int count = 0;
                                foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                    if (item.Selected) ++count;
                                dayOfWeekTask.DayOfWeek = new DayOfWeek[count];
                                count = 0;
                                foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                {
                                    if (item.Selected)
                                    {
                                        dayOfWeekTask.DayOfWeek[count] = (DayOfWeek)int.Parse(item.Value);
                                        ++count;
                                    }
                                }
                                task = dayOfWeekTask;
                            }
                            break;
                    }

                    TheAdminServer.ScheduledTaskManager.Add(task, name, comment);
                }
                else if (_opType == PageOpType.Modify)
                {
                    IScheduledTask task = null;
                    //如果类型没有改变，则不需要生成新的对象；否则，需要根据类型生成新的对象
                    if (_taskUnit.Task.Type == type)
                    {
                        task = _taskUnit.Task;

                        switch (type)
                        {
                            case ScheduledTaskType.Once:
                                {
                                    OnceScheduledTask onceTask = (OnceScheduledTask)task;
                                    onceTask.Automation = automation;
                                    onceTask.ExecuteTime = DateTime.Parse(TextBoxExecuteTime.Text);
                                }
                                break;
                            case ScheduledTaskType.Repeat:
                                {
                                    RepeatScheduledTask repeatTask = (RepeatScheduledTask)task;
                                    repeatTask.Automation = automation;
                                    repeatTask.StartTime = TimeSpan.Parse(TextBoxStartTime.Text);
                                    repeatTask.EndTime = TimeSpan.Parse(TextBoxEndTime.Text);
                                    repeatTask.Interval = TimeSpan.Parse(TextBoxInterval.Text);
                                }
                                break;
                            case ScheduledTaskType.DayOfWeek:
                                {
                                    DayOfWeekScheduledTask dayOfWeekTask = task as DayOfWeekScheduledTask;
                                    dayOfWeekTask.Automation = automation;
                                    dayOfWeekTask.TimeOfDay = TimeSpan.Parse(TextBoxDayOfWeekExecuteTime.Text);
                                    int count = 0;
                                    foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                        if (item.Selected) ++count;
                                    dayOfWeekTask.DayOfWeek = new DayOfWeek[count];
                                    count = 0;
                                    foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                    {
                                        if (item.Selected)
                                        {
                                            dayOfWeekTask.DayOfWeek[count] = (DayOfWeek)int.Parse(item.Value);
                                            ++count;
                                        }
                                    }
                                }
                                break;
                        }
                    }
                    else
                    {
                        switch (type)
                        {
                            case ScheduledTaskType.Once:
                                {
                                    OnceScheduledTask onceTask = new OnceScheduledTask();
                                    onceTask.Automation = automation;
                                    onceTask.ExecuteTime = DateTime.Parse(TextBoxExecuteTime.Text);
                                    task = onceTask;
                                }
                                break;
                            case ScheduledTaskType.Repeat:
                                {
                                    RepeatScheduledTask repeatTask = new RepeatScheduledTask();
                                    repeatTask.Automation = automation;
                                    repeatTask.StartTime = TimeSpan.Parse(TextBoxStartTime.Text);
                                    repeatTask.EndTime = TimeSpan.Parse(TextBoxEndTime.Text);
                                    repeatTask.Interval = TimeSpan.Parse(TextBoxInterval.Text);
                                    task = repeatTask;
                                }
                                break;
                            case ScheduledTaskType.DayOfWeek:
                                {
                                    DayOfWeekScheduledTask dayOfWeekTask = new DayOfWeekScheduledTask();
                                    dayOfWeekTask.Automation = automation;
                                    dayOfWeekTask.TimeOfDay = TimeSpan.Parse(TextBoxDayOfWeekExecuteTime.Text);
                                    int count = 0;
                                    foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                        if (item.Selected) ++count;
                                    dayOfWeekTask.DayOfWeek = new DayOfWeek[count];
                                    count = 0;
                                    foreach (ListItem item in CheckBoxListDayOfWeek.Items)
                                    {
                                        if (item.Selected)
                                        {
                                            dayOfWeekTask.DayOfWeek[count] = (DayOfWeek)int.Parse(item.Value);
                                            ++count;
                                        }
                                    }
                                    task = dayOfWeekTask;
                                }
                                break;
                        }
                    }

                    TheAdminServer.ScheduledTaskManager.Update(_taskUnit.Task.SecurityObject.Id, task, name, comment);
                }

                Response.Redirect("ScheduledTaskList.aspx");
            }
            else
            {
                OperateResult.Text = "未指定要执行的自动化任务（需要在第一步中指定并点击'应用'按钮）";

                return;
            }
		}
		catch (Exception ex)
		{
            OperateResult.Text = StringDef.ParameterInputError +StringDef.Colon+ ex.Message;
            //Response.Write("<script language='JavaScript'>alert('参数有误!');</script>");
            //ShowMessage("参数有误", MessageType.Failure);
		}
	}

	void SetScheduledTaskType(ScheduledTaskType type)
	{
		for (int i = 0; i < DropDownListScheduledTaskType.Items.Count; i++)
		{
			ListItem item = DropDownListScheduledTaskType.Items[i];
			if (int.Parse(item.Value) == (int)type)
			{
				DropDownListScheduledTaskType.SelectedIndex = i;
				ShowSelectedTaskTypeOption();
				break;
			}
		}
	}

	void ShowSelectedTaskTypeOption()
	{
		PanelOnce.Visible = false;
		PanelRepeat.Visible = false;
        PanelDayOfWeek.Visible = false;
        
		ScheduledTaskType type = ScheduledTaskType.Once;
		try
		{
			type = (ScheduledTaskType)(int.Parse(DropDownListScheduledTaskType.SelectedValue));
		}
		catch (Exception)
		{
		}
		switch (type)
		{
			case ScheduledTaskType.Once:
				PanelOnce.Visible = true;
				break;
			case ScheduledTaskType.Repeat:
				PanelRepeat.Visible = true;
				break;
            case ScheduledTaskType.DayOfWeek:
                PanelDayOfWeek.Visible = true;
                break;
		}
	}

	protected void ListBoxScheduledTaskType_SelectedIndexChanged(object sender, EventArgs e)
	{
		ShowSelectedTaskTypeOption();
	}
}
