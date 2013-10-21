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

public partial class GameServer_AddAlertCondition : BasePage
{
	public string[] AlertConditionNames = { StringDef.CpuUsageAlert, StringDef.FreeDiskSpaceAlert };

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAlertCondition, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (!IsPostBack)
		{
			for(int i = 0; i < AlertConditionNames.Length; i++)
			{
				ListBoxAlertCondition.Items.Add(AlertConditionNames[i]);
			}

			ListBoxAlertCondition.SelectedIndex = 0;
			RefreshParams();
		}
	}

	public AlertCondition MakeAlertCondition(int index, string param1, string param2)
	{
		switch(index)
		{
			case 0:
				return new CpuUsageAlertCondition(int.Parse(param1));
			case 1:
				return new DiskUsageAlertCondition((DiskUsageAlertCondition.Mode)(int.Parse(param1)), long.Parse(param2));
		}

		return null;
	}

	protected void LinkButtonAdd_Click(object sender, EventArgs e)
	{
		try
		{
            //添加需要写权限
            if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameServerAlertCondition, OpType.WRITE, Session))
            {
                LabelOpMsg.Text = StringDef.NotEnoughPrivilege;
                return;                
            }

			AlertCondition condition = MakeAlertCondition(ListBoxAlertCondition.SelectedIndex, TextBoxParam1.Text, TextBoxParam2.Text);
			if (condition != null)
			{
				TheAdminServer.GameServerMonitor.AlertConditions.Add(condition);

				//ShowMessage(StringDef.AddAlertCondition + StringDef.Colon + StringDef.Success, MessageType.Success);
				Response.Redirect("AlertConfig.aspx");
			}
		}
		catch (Exception)
		{
			ShowMessage(StringDef.AddAlertCondition + StringDef.Colon + StringDef.Failure, MessageType.Failure);
		}
	}

	protected void ListBoxAlertCondition_SelectIndexChanged(object sender, EventArgs e)
	{
		RefreshParams();
	}

	private void RefreshParams()
	{
		switch (ListBoxAlertCondition.SelectedIndex)
		{
			case 0:
				LiteralParam1.Text = StringDef.CpuUsageAlertValue;
				LiteralParam2.Text = string.Empty;				
				TextBoxParam1.Visible = true;
				TextBoxParam2.Visible = false;
				break;
			case 1:				
				LiteralParam1.Text = StringDef.FreeDiskSpaceAlertValueType;
				LiteralParam2.Text = StringDef.FreeDiskSpaceAlertValue;
				TextBoxParam1.Visible = true;
				TextBoxParam2.Visible = true;
				break;
		}		
	}
}
