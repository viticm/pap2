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

public partial class Security_EditPrivilege : BasePage
{
	FSEyeObject _object;

	public int ObjectId
	{
		get { return _object.Id; }
	}

	protected void Page_Load(object sender, EventArgs e)
	{        
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionSecurityEditPrivilege, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		if (Request.Params[WebConfig.ParamObjectId] != null)
		{
			try
			{
				int objectId = int.Parse(Request.Params[WebConfig.ParamObjectId]);
				_object = AdminServer.TheInstance.SecurityManager.Get(objectId);

                ////修改需要Write权限
                //if (_object != null && !WebUtil.CheckPrivilege(_object, OpType.WRITE, Session))
                //{
                //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
                //}

				if (_object != null)
					LabelEditObject.Text = StringDef.EditObject + StringDef.Colon + _object.FullPath;
                if (!IsPostBack) LabelTitle.Text += "["+_object.Name+"]";
			}
			catch (Exception)
			{
				//Response.Redirect("Security.aspx");
			}
		}
		else
		{
			Response.Redirect("Security.aspx");
		}

		SystemObjectTree1.SelectedObjectChanged += new EventHandler(SystemObjectTree1_SelectedObjectChanged);
        //System.Threading.Thread.Sleep(500);

        FSEyeObject selObj = SystemObjectTree1.SelectedObject;
        if (selObj != null)
        {
            LiteralSelectedNode.Text = selObj.FullPath;
        }
        else
        {
            LiteralSelectedNode.Text = StringDef.None;
        }
	}

	void RefreshCurrentPrivilege()
	{
		FSEyeObject selectedObject = SystemObjectTree1.SelectedObject;
		if (selectedObject != null)
		{
			int objectId = _object.Id;
			AccessControlEntry[] aces = selectedObject.Aces;

            CheckBoxAllowRead.Checked = false;
            CheckBoxAllowWrite.Checked = false;
            CheckBoxAllowExecute.Checked = false;
			foreach (AccessControlEntry ace in aces)
			{
				if (ace.OperatorId == objectId)
				{
                    CheckBoxAllowRead.Checked = (ace.Operation & OpType.READ) > 0;
                    CheckBoxAllowWrite.Checked = (ace.Operation & OpType.WRITE) > 0;
                    CheckBoxAllowExecute.Checked = (ace.Operation & OpType.EXECUTE) > 0;
				}
			}
		}
	}

	protected void SystemObjectTree1_SelectedObjectChanged(object sender, EventArgs e)
	{
		RefreshCurrentPrivilege();
	}

	protected void ButtonModify_Click(object sender, EventArgs e)
	{
        //需要修改权限
        if (!WebUtil.CheckPrivilege(_object, OpType.WRITE, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

		ButtonModify.Enabled = false;
		ButtonSave.Enabled = true;
		ButtonCancel.Enabled = true;
        RadioButtonListApplyAll.Enabled = true;

		PanelAce.Enabled = true;
	}

	protected void ButtonSave_Click(object sender, EventArgs e)
	{
        //if (!WebUtil.CheckPrivilege(_object, OpType.WRITE, Session))
        //{
        //    Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        //}
		ButtonModify.Enabled = true;
		ButtonSave.Enabled = false;
		ButtonCancel.Enabled = false;
        RadioButtonListApplyAll.Enabled = false;

		FSEyeObject selectedObject = SystemObjectTree1.SelectedObject;
		if (selectedObject == null)
			return;

		int opType = 0;
		if (CheckBoxAllowRead.Checked)
		{
			opType |= OpType.READ;
		}
		if (CheckBoxAllowWrite.Checked)
		{
			opType |= OpType.WRITE;
		}
		if (CheckBoxAllowExecute.Checked)
		{
			opType |= OpType.EXECUTE;
		}

		if (!WebUtil.CheckPrivilege(_object.Id, OpType.WRITE, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}
        bool applyAll = RadioButtonListApplyAll.SelectedIndex == 0 ? true : false;

        SetAce(selectedObject, opType,applyAll);
		PanelAce.Enabled = false;
	}
    
    /// <summary>
    /// 设置权限
    /// </summary>
    /// <param name="fsObject">FSEyeObject</param>
    /// <param name="opType">操作</param>
    void SetAce(FSEyeObject fsObject,int opType,bool applyAll)
    {
        fsObject.AddAce(new AccessControlEntry(_object.Id, opType),true);
        //保存
        AdminServer.TheInstance.SecurityManager.Set(fsObject);
        if (!applyAll||fsObject.Children == null) return;
        foreach (FSEyeObject child in fsObject.Children)
        {
            SetAce(child, opType,applyAll);
        }
    }

	protected void ButtonCancel_Click(object sender, EventArgs e)
	{
		RefreshCurrentPrivilege();

		ButtonModify.Enabled = true;
		ButtonSave.Enabled = false;
		ButtonCancel.Enabled = false;
        RadioButtonListApplyAll.Enabled = false;

		PanelAce.Enabled = false;
	}
}