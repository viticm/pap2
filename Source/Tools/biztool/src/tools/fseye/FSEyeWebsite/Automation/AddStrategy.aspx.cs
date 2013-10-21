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
using FSEye;
using FSEye.Security;

public partial class GameServer_AddStrategy : BasePage
{
	enum PageOpType
	{
		Add,
		Modify		
	}

	PageOpType _opType;

	Strategy _strategy;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationAddStrategy, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		string op = Request.Params[WebConfig.ParamOperation];
		if (op != null && op == WebConfig.OpTypeModify && Session[WebConfig.SessionStrategy] != null)
		{
			_opType = PageOpType.Modify;
			_strategy = Session[WebConfig.SessionStrategy] as Strategy;
            //要修改策略需要在StrategyManager上有写权限
            if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
		}
		else
		{
			_opType = PageOpType.Add;
            //要添加策略需要在StrategyManager上有写权限
            if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }
		}

		if (!IsPostBack)
		{			
			if (_opType == PageOpType.Modify)
			{
				AutomationEditorStrategy.Automation = _strategy.Automation;
				TextBoxName.Text = _strategy.SecurityObject.Name;
				TextBoxComment.Text = _strategy.SecurityObject.Comment;
				LabelOpType.Text = StringDef.ModifyStrategy;
			}
			else if (_opType == PageOpType.Add)
			{
				AutomationEditorStrategy.Automation = null;
				LabelOpType.Text = StringDef.AddStrategy;
			}
			ListEvent();
		}

        LabelSuccess.Visible = false;
        LabelOpMsg.Visible = false;
	}

	private void ListEvent()
	{
		IList<FSEyeEvent> events = TheAdminServer.StrategyManager.EventList;
		for (int i = 0; i < events.Count; i++)
		{
			FSEyeEvent fsEyeEvent = events[i];
			DropDownListEvent.Items.Add(new ListItem(fsEyeEvent.ToString(), i.ToString()));

			if (_strategy != null && fsEyeEvent == _strategy.Event)
			{
				DropDownListEvent.SelectedIndex = i;
			}
		}
	}

	protected void FinishButton_Click(object sender, EventArgs e)
	{
		IAutomation automation = AutomationEditorStrategy.Automation;

        if (automation == null)
        {
            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure + StringDef.Colon + "未指定要执行的自动化任务（需要在第一步中指定并点击'应用'按钮）";

            return;
        }

        string name = TextBoxName.Text.Trim();
        string comment = TextBoxComment.Text.Trim();

        foreach (Strategy strategyInList in TheAdminServer.StrategyManager.StrategyList)
        {
            if (strategyInList.SecurityObject.Name == name)
            {
                //重名的判定：
                //1.添加时与任何一个已有的策略名称相同
                //2.修改时与除本身外的任何一个已有的策略名称相同
                if ((_opType == PageOpType.Add) || ((_opType == PageOpType.Modify) && (strategyInList.SecurityObject.Id != _strategy.SecurityObject.Id)))
                {
                    LabelOpMsg.Visible = true;
                    LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure + StringDef.Colon + "该策略名称已存在" + StringDef.Colon + "策略ID - " + strategyInList.SecurityObject.Id.ToString();

                    return;
                }
            }
        }

		if (_opType == PageOpType.Add)
		{
			IList<FSEyeEvent> events = TheAdminServer.StrategyManager.EventList;
			int index = DropDownListEvent.SelectedIndex;
			if (index >= 0)
			{
				FSEyeEvent fseyeEvent = events[index];

				Strategy strategy = new Strategy();
				strategy.Event = fseyeEvent;
				strategy.Automation = automation;

                try
                {
                    if (TheAdminServer.StrategyManager.Add(strategy, name, comment))
                    {
                        LabelSuccess.Visible = true;
                        LabelSuccess.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Success;
                        Response.Redirect("StrategyList.aspx");
                        return;
                    }
                }
                catch (Exception ex)
                {
                    LabelOpMsg.Visible = true;
                    LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure + StringDef.Colon + ex.ToString();

                    return;
                }
			}			

            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure;
		}
		else if (_opType == PageOpType.Modify)
		{
			IList<FSEyeEvent> events = TheAdminServer.StrategyManager.EventList;
			int index = DropDownListEvent.SelectedIndex;
			if (index >= 0)
			{
				FSEyeEvent fseyeEvent = events[index];

				_strategy.Event = fseyeEvent;
				_strategy.Automation = automation;

                try
                {
                    TheAdminServer.StrategyManager.Update(_strategy.SecurityObject.Id, _strategy, name, comment);
                    Response.Redirect("StrategyList.aspx");
                    return;
                }
                catch (Exception ex)
                {
                    LabelOpMsg.Visible = true;
                    LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure + StringDef.Colon + ex.ToString();

                    return;
                }
			}			

            LabelOpMsg.Visible = true;
            LabelOpMsg.Text = StringDef.AddStrategy + StringDef.Colon + StringDef.Failure;
		}
	}
}
