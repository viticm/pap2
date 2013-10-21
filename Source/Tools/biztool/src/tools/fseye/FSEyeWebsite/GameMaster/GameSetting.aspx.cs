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
using FSEye.FS2Lib;

public partial class GameMaster_GameSetting : BasePage
{
	IList _globalVariableTextBoxList = new ArrayList();

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterGameSetting, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		_globalVariableTextBoxList.Clear();
		_globalVariableTextBoxList.Add(TextBoxGlobalVariable1);
		_globalVariableTextBoxList.Add(TextBoxGlobalVariable2);
		_globalVariableTextBoxList.Add(TextBoxGlobalVariable3);
		_globalVariableTextBoxList.Add(TextBoxGlobalVariable4);

		if (!IsPostBack)
		{
			//ShowGameSettings();
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
		}
        LabelOpMsg.Text = string.Empty;
	}

    //protected void ServerDropDownList_ServerIndexChanged(object sender, EventArgs e)
    //{
    //    ShowGameSettings();
    //}

    protected void LinkButtonShow_Click(object sender, EventArgs e)
    {
        ShowGameSettings();
    }

	private void ShowGameSettings()
	{
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }

            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }

            for (ushort i = 0; i < 4; i++)
            {
                server.GameSetting.RefreshGlobalVariable(i);
            }

            int[] globalVariableArray = server.GameSetting.GlobalVariable;

            for (int i = 0; i < 4; i++)
            {
                (_globalVariableTextBoxList[i] as TextBox).Text = globalVariableArray[i].ToString();
            }
        }
        catch (Exception)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.Show + StringDef.GameSetting;
        }
	}

	protected void LinkButtonApply_Click(object sender, EventArgs e)
    {
        try
        {
            GameServer server = ServerDropDownList.SelectedGameServer;
            if (server == null)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
                return;
            }

            if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.WRITE, Session))
            {
                Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
            }

            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return;
            }

            GameSetting setting = server.GameSetting;
            IList<int> globalVariableList = setting.GlobalVariable;

            for (int i = 0; i < globalVariableList.Count; i++)
            {
                int newValue = int.Parse((_globalVariableTextBoxList[i] as TextBox).Text);
                if (newValue != globalVariableList[i])
                {
                    setting.SetGlobalVariable((ushort)i, newValue);
                }
            }
        }
        catch (Exception)
        {
            LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.Apply + StringDef.GameSetting;
        }
	}

    protected void SelectedServerGroupChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }
}
