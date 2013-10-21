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
using System.Text;
using Resources;
using FSEye;
using FSEye.Security;

public partial class GameServer_StrategyList : BasePage
{
	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationStrategyList, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			ListStrategy();
		}

		ShowStrategyCount();
		RefreshButtonSwitchStrategyOn();
        LabelOpMsg.Text = string.Empty;
	}

	private void ShowStrategyCount()
	{
		LabelStrategySummary.Text = string.Format(StringDef.TotalStrategyCount, TheAdminServer.StrategyManager.StrategyList.Count);
	}

	private void ListStrategy()
	{
		ListItemCollection list = ListBoxStrategy.Items;
		list.Clear();

		IList strategyList = TheAdminServer.StrategyManager.StrategyList;
		for (int i = 0; i < strategyList.Count; i++)
		{
			Strategy strategy = strategyList[i] as Strategy;
			string itemText = strategy.ToString();
			if (!strategy.Enabled)
			{
				itemText += " [" + StringDef.Disable + "]";
			}
			list.Add(new ListItem(itemText, strategy.SecurityObject.Id.ToString()));
		}
	}

	protected void ButtonDel_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxStrategy.Items)
		{
			if (item.Selected)
			{
				int strategyId = int.Parse(item.Value);
                //要删除策略需要在StrategyManager上有写权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.WRITE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }

				TheAdminServer.StrategyManager.Delete(strategyId);
			}
		}

		ListStrategy();
		ShowStrategyCount();
	}

	protected void ButtonModify_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxStrategy.Items)
		{
			if (item.Selected)
			{
				int strategyId = int.Parse(item.Value);
				Strategy strategy = TheAdminServer.StrategyManager.Get(strategyId);
				if (strategy != null)
				{
					Session[WebConfig.SessionStrategy] = strategy;
					Response.Redirect("AddStrategy.aspx?" + WebConfig.ParamOperation + "=" + WebConfig.OpTypeModify);
				}
			}
		}
	}

	protected void ButtonEnable_Click(object sender, EventArgs e)
	{
		foreach(ListItem item in ListBoxStrategy.Items)
		{
			if (item.Selected)
			{
				int strategyId = int.Parse(item.Value);
                //要启用策略需要在StrategyManager上有执行权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
				TheAdminServer.StrategyManager.Enable(strategyId, true);
			}
		}

		ListStrategy();
	}

	protected void ButtonDisable_Click(object sender, EventArgs e)
	{
		foreach (ListItem item in ListBoxStrategy.Items)
		{
			if (item.Selected)
			{
				int strategyId = int.Parse(item.Value);
                //要禁用策略需要在StrategyManager上有执行权限
                if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.EXECUTE, Session))
                {
                    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                }
				TheAdminServer.StrategyManager.Enable(strategyId, false);
			}
		}

		ListStrategy();
	}

	protected void ButtonTestEvent_Click(object sender, EventArgs e)
	{
		TheAdminServer.StrategyManager.OnEvent(FSEyeEvent.TestEvent, new AutomationContext());
	}

	protected void ButtonSwitchStrategyOn_Click(object sender, EventArgs e)
	{
        //要开启和关闭策略模块需要在StrategyManager上有执行权限(Object)
        if (!WebUtil.CheckPrivilege(TheAdminServer.StrategyManager.SecurityObject, OpType.EXECUTE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }
		TheAdminServer.StrategyManager.On = !TheAdminServer.StrategyManager.On;
		RefreshButtonSwitchStrategyOn();
	}

	private void RefreshButtonSwitchStrategyOn()
	{
		if (TheAdminServer.StrategyManager.On)
		{
			LabelStrategyOn.Text = "策略正在运行";
			LabelStrategyOn.CssClass = CssConfig.MessageSuccess;
			ButtonSwitchStrategyOn.Text = "关闭所有策略";
		}
		else
		{
			LabelStrategyOn.Text = "策略已经关闭";
			LabelStrategyOn.CssClass = CssConfig.MessageFailure;
			ButtonSwitchStrategyOn.Text = "开启所有策略";
		}
	}

    protected void LinkButtonExport_Click(object sender, EventArgs e)
    {
        string fileName = SystemConfig.Current.AdminServerUploadFileRootPath + WebConfig.FileStrategyList;

        ArrayList strategyToExport = new ArrayList();
        foreach (ListItem item in ListBoxStrategy.Items)
        {
            if (item.Selected)
            {
                int strategyId = int.Parse(item.Value);
                Strategy strategy = TheAdminServer.StrategyManager.Get(strategyId);
                if (strategy != null) strategyToExport.Add(strategy);
            }
        }

        if (strategyToExport != null && strategyToExport.Count != 0)
        {
            if (WebUtil.ExportStrategy(fileName, strategyToExport))
                LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + string.Format(StringDef.MsgAutomationExport, fileName, StringDef.StrategyList);
            else
                LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + string.Format(StringDef.MsgAutomationExport, fileName, StringDef.StrategyList);
        }
    }

    protected void LinkButtonImport_Click(object sender, EventArgs e)
    {
        string fileName = SystemConfig.Current.AdminServerUploadFileRootPath + WebConfig.FileStrategyList;
        if (WebUtil.ImportStrategy(fileName))
            LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + string.Format(StringDef.MsgAutomationImport, fileName, StringDef.StrategyList);
        else
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + string.Format(StringDef.MsgAutomationImport, fileName, StringDef.StrategyList);

        ListStrategy();
    }
}
