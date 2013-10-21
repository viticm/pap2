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

public partial class Automation_EditBatchTask : BasePage
{
    BatchTask _task;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionAutomationEditBatchTask, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if ((Request.Params[WebConfig.ParamOperation] as string) == "modify")
        {
            int taskId = int.Parse(Request.Params[WebConfig.ParamTaskId]);
            _task = TheAdminServer.BatchTaskManager.GetBatchTask(taskId);
        }

        if (!IsPostBack)
        {
            SetBatchTask(_task);
        }
    }

    void SetBatchTask(BatchTask task)
    {
        if (task != null)
        {
            int[] serverIds = task.Servers;
            foreach (int serverId in serverIds)
            {
                GameServerCheckBoxList1.SetServerSelected(serverId, true);
            }

            TextBoxName.Text = task.SecurityObject.Name;
            TextBoxDesc.Text = task.SecurityObject.Comment;
            AutomationEditor1.Automation = task.Automation;
            foreach (ListItem item in DropDownListStepSize.Items)
            {
                if (item.Value == task.Step.ToString())
                {
                    item.Selected = true;
                    break;
                }
            }
            LinkButtonApply.Text = StringDef.Modify;
        }
        else
        {
            TextBoxDesc.Text = StringDef.Untitled;
            AutomationEditor1.Automation = null;
            LinkButtonApply.Text = StringDef.Add;
        }
    }

    protected void LinkButtonApply_Click(object sender, EventArgs e)
    {
        try
        {
            if (!WebUtil.CheckPrivilege(TheAdminServer.BatchTaskManager.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            GameServer[] selectedServers = GameServerCheckBoxList1.SelectedGameServers;
            if (selectedServers == null || selectedServers.Length <= 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }
            IAutomation automation = AutomationEditor1.Automation;
            if (automation == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Automation);
                return;
            }

            string name = TextBoxName.Text.Trim();
            if (name.Length == 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Name);
                return;
            }

            string desc = TextBoxDesc.Text.Trim();
            if (desc.Length == 0)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Description);
                return;
            }

            if (_task == null)
            {
                //Ìí¼Ó
                _task = new BatchTask();
                _task.Step = int.Parse(DropDownListStepSize.SelectedValue);
                foreach (GameServer server in selectedServers)
                {
                    if (server != null)
                    {
                        _task.AddServer(server.Id);
                    }
                }
                _task.Automation = automation;

                if (TheAdminServer.BatchTaskManager.AddTask(_task, name, desc))
                {
                    Response.Redirect("BatchTask.aspx", true);                    
                }
                else
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.AddTask;
                    return;
                }
            }
            else
            {
                //ÐÞ¸Ä
                if (TheAdminServer.BatchTaskManager.EditTask(_task.ID,
                        int.Parse(DropDownListStepSize.SelectedValue),
                        automation,
                        selectedServers,
                        name,
                        desc))
                {
                    Response.Redirect("BatchTask.aspx", true);
                }
                else
                {
                    LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.EditBatchTask;
                    return;
                }
            }
        }
        catch (Exception ex)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + ex.Message;
        }
    }
}