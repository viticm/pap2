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
using FSEye.Security;
using FSEye.FS2Lib;
using FSEye.PlugIn;
using FSEye;

public partial class GameMaster_RoleDetail : BasePage
{
	private FS2RoleDataInfo _role;

	private GameServer _server;

	protected void Page_Load(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterRoleDetail, OpType.READ, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
		}

		try
		{
			ClearMessage();

			if (!IsPostBack)
			{
				if (Request.Params[WebConfig.ParamServerId] != null)
				{
					int serverId = int.Parse(Request.Params[WebConfig.ParamServerId]);
                    ViewState[WebConfig.ParamServerId] = serverId;

					object queryRoleArg = null;
					if (Request.Params[WebConfig.ParamRoleId] != null)
					{
						queryRoleArg = int.Parse(Request.Params[WebConfig.ParamRoleId]);
					}
					else if (Request.Params[WebConfig.ParamRoleName] != null)
					{
                        //传RoleName时需要对其编码， 否则中文名有时不正常。
						queryRoleArg = Server.UrlDecode(Request.Params[WebConfig.ParamRoleName]);
					}

					_server = AdminServer.TheInstance.GameServerManager.GetGameServer(serverId);

					if (_server == null)
						return;

                    if (!WebUtil.CheckPrivilege(_server.SecurityObject, OpType.READ, Session))
                    {
                        Response.Redirect(WebConfig.PageNotEnoughPrivilege);
                    }

					_role = SetRoleData(_server, queryRoleArg);
                    if (_role == null) throw new Exception();

					Session["role"] = _role;
					Session["server"] = _server;

                    LinkRoleItemInfo.NavigateUrl = string.Format("RoleItemInfo.aspx?{0}={1}&{2}={3}",
                        WebConfig.ParamServerId, serverId, WebConfig.ParamRoleId, _role.RoleId);
                    LinkRoleSkillInfo.NavigateUrl = string.Format("RoleSkillInfo.aspx?{0}={1}&{2}={3}",
                        WebConfig.ParamServerId, serverId, WebConfig.ParamRoleId, _role.RoleId);
                    LinkExpStatistic.NavigateUrl = string.Format("~/GameInfo/ExpStatistic.aspx?{0}={1}&{2}={3}",
                        WebConfig.ParamServerId, serverId, WebConfig.ParamRoleName, Server.UrlEncode(_role.RoleName));
                    LinkExecuteGMCommand.NavigateUrl = string.Format("GMCommand.aspx?{0}={1}&{2}={3}",
                        WebConfig.ParamServerId, serverId, WebConfig.ParamRoleName, Server.UrlEncode(_role.RoleName));

					LinkDownloadData.NavigateUrl = string.Format("GameMasterHandler.ashx?{0}={1}&{2}={3}&{4}={5}", WebConfig.ParamRoleId, _role.RoleId, WebConfig.ParamServerId, serverId, WebConfig.ParamOperation, "GetRoleDataSql");
                    LinkViewAccountInfo.NavigateUrl = string.Format("~/Paysys/AccountInfo.aspx?{0}={1}",
                                WebConfig.ParamAccount, _role.AccountName);

					SetRoleBasicInfo(_role);	
				
                    //操作的LinkButton的Text
                    LinkButtonTalk.Text = _role.NoChatIn == 0 ? StringDef.ChatDisable : StringDef.ChatEnable;
                    LinkButtonFreeze.Text = _role.NoLoginIn == 0 ? StringDef.Freeze : StringDef.Unfreeze;
                    LinkButtonSave.Text = StringDef.SaveByForce;
                    LinkButtonQuit.Text = StringDef.QuitByForce;
                    LinkClanInfo.Text = StringDef.Clan + StringDef.Information;
                    if (_role.Online == 0)
                    {
                        LinkButtonSave.Enabled = false;
                        LinkButtonQuit.Enabled = false;
                    }
				}
				else
				{
					Response.Redirect("QueryRole.aspx");
				}
			}
			else
			{
                _role = Session["role"] as FS2RoleDataInfo;
				_server = Session["server"] as GameServer;
			}
		}
		catch (Exception)
		{
			Response.Redirect("QueryRole.aspx");
		}

	}

    private FS2RoleDataInfo SetRoleData(GameServer server, object arg)
	{		
		if (server != null)
		{
			SqlCommand cmd = null;
			if (arg.GetType() == typeof(int))
			{
				cmd = new SqlCommand(
                    string.Format("SELECT {0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19},{20},{21},{24},{25} FROM {22} WHERE {23}='?'",
						FS2TableString.RolesfirstFieldId,
						FS2TableString.RolesfirstFieldTongGUID,
						FS2TableString.RolesfirstFieldAccountName,
						FS2TableString.RolesfirstFieldRoleName,
						FS2TableString.RolesfirstFieldRoleSex,
						FS2TableString.RolesfirstFieldRoleType,
						FS2TableString.RolesfirstFieldRoleLevel,
						FS2TableString.RolesfirstFieldMoney,
						FS2TableString.RolesfirstFieldMoneyInBox,
						FS2TableString.RolesfirstFieldPlayedTime,
						FS2TableString.RolesfirstFieldCreateDate,
						FS2TableString.RolesfirstFieldLastPlayingDate,
						FS2TableString.RolesfirstFieldLastPlayingIP,
						FS2TableString.RolesfirstFieldNoChatIn,
						FS2TableString.RolesfirstFieldNoLoginIn,
						FS2TableString.RolesfirstFieldUseRevivePosition,
						FS2TableString.RolesfirstFieldReviveID,
						FS2TableString.RolesfirstFieldReviveX,
						FS2TableString.RolesfirstFieldReviveY,
						FS2TableString.RolesfirstFieldEnterMapID,
						FS2TableString.RolesfirstFieldGUID,
                        FS2TableString.RolesfirstFieldOnline,
						FS2TableString.RolesfirstTableName,
						FS2TableString.RolesfirstFieldId,
                        FS2TableString.RolesfirstFieldSkillSeries,
                        FS2TableString.RolesfirstFieldSpyLevel),
					 arg
					);
			}
			else if (arg.GetType() == typeof(string))
			{
				cmd = new SqlCommand(
					string.Format("SELECT {0},{1},{2},{3},{4},{5},{6},{7},{8},{9},{10},{11},{12},{13},{14},{15},{16},{17},{18},{19},{20},{21},{24},{25} FROM {22} WHERE {23}='?'",
						FS2TableString.RolesfirstFieldId,
						FS2TableString.RolesfirstFieldTongGUID,
						FS2TableString.RolesfirstFieldAccountName,
						FS2TableString.RolesfirstFieldRoleName,
						FS2TableString.RolesfirstFieldRoleSex,
						FS2TableString.RolesfirstFieldRoleType,
						FS2TableString.RolesfirstFieldRoleLevel,
						FS2TableString.RolesfirstFieldMoney,
						FS2TableString.RolesfirstFieldMoneyInBox,
						FS2TableString.RolesfirstFieldPlayedTime,
						FS2TableString.RolesfirstFieldCreateDate,
						FS2TableString.RolesfirstFieldLastPlayingDate,
						FS2TableString.RolesfirstFieldLastPlayingIP,
						FS2TableString.RolesfirstFieldNoChatIn,
						FS2TableString.RolesfirstFieldNoLoginIn,
						FS2TableString.RolesfirstFieldUseRevivePosition,
						FS2TableString.RolesfirstFieldReviveID,
						FS2TableString.RolesfirstFieldReviveX,
						FS2TableString.RolesfirstFieldReviveY,
						FS2TableString.RolesfirstFieldEnterMapID,
						FS2TableString.RolesfirstFieldGUID,                        
                        FS2TableString.RolesfirstFieldOnline,
						FS2TableString.RolesfirstTableName,
						FS2TableString.RolesfirstFieldRoleName,
                        FS2TableString.RolesfirstFieldSkillSeries,
                        FS2TableString.RolesfirstFieldSpyLevel),
					 arg
					);
			}

            if (!server.IsConnected)
            {
                LabelOpMsg.Text = StringDef.NoConnectionAlert;
                return null;
            }
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
			if (result != null)
			{
				if (result.Success)
				{
					result.SetFieldType(
						new SqlDataType[] {
							SqlDataType.Int32,
							SqlDataType.String,
							SqlDataType.String,
							SqlDataType.String,
							SqlDataType.SByte,
							SqlDataType.SByte,
							SqlDataType.SByte,
							SqlDataType.UInt32,
							SqlDataType.UInt32,
							SqlDataType.UInt32,
							SqlDataType.DateTime,
							SqlDataType.DateTime,
							SqlDataType.Int64,
							SqlDataType.Int32,
							SqlDataType.Int32,
							SqlDataType.Byte,
							SqlDataType.Int32,
							SqlDataType.Int32,
							SqlDataType.Int32,
							SqlDataType.Int32,
							SqlDataType.String,
                            SqlDataType.Int32,
                            SqlDataType.Int32,
                            SqlDataType.Int32
						});

					object[] record = result.ReadRecord();
					if (record != null)
					{
                        FS2RoleDataInfo role = new FS2RoleDataInfo();
						role.RoleId = (int)record[0];
						role.TongName = (string)record[1];
						role.AccountName = (string)record[2];
						role.RoleName = (string)record[3];
						role.RoleSex = (FS2RoleSex)(int.Parse(record[4].ToString()));
                        role.SkillSeries = (int)record[22];
                        switch ((SByte)record[5])
                        {
                            case (SByte)0:
                                if (role.SkillSeries == 0) role.RoleType = FS2RoleType.Xuanfeng;
                                else if (role.SkillSeries == 1) role.RoleType = FS2RoleType.Xingtian;
                                else role.RoleType = FS2RoleType.Jiashi;
                                break;
                            case (SByte)1:
                                if (role.SkillSeries == 0) role.RoleType = FS2RoleType.Zhenren;
                                else if (role.SkillSeries == 1) role.RoleType = FS2RoleType.Tianshi;
                                else role.RoleType = FS2RoleType.Daoshi;
                                break;
                            case (SByte)2:
                                if (role.SkillSeries == 0) role.RoleType = FS2RoleType.Shoushi;
                                else if (role.SkillSeries == 1) role.RoleType = FS2RoleType.Yishi;
                                else role.RoleType = FS2RoleType.Yiren;
                                break;
                        }
						role.RoleLevel = int.Parse(record[6].ToString());
						role.Money = (uint)record[7];
						role.MoneyInBox = (uint)record[8];
						role.PlayedTime = (uint)record[9];
						role.CreateDate = (DateTime)record[10];
						role.LastPlayingDate = (DateTime)record[11];
						role.LastPlayingIP = new System.Net.IPAddress((Int64)record[12]);
						role.NoChatIn = (int)record[13];
						role.NoLoginIn = (int)record[14];
						role.UseRevivePosition = (byte)record[15];
						role.ReviveID = (int)record[16];
						role.ReviveX = (int)record[17];
						role.ReviveY = (int)record[18];
						role.EnterMapId = (int)record[19];
						role.RoleGuid = (string)record[20];
                        role.Online = (int)record[21];                        
                        role.SpyLevel = (int)record[23];

                        //查询氏族名称
                        if (role.TongName != null && role.TongName.Length != 0)
                        {
                            SqlCommand tongNameCmd = new SqlCommand(string.Format("SELECT {0} FROM {1} WHERE {2}='?'",
                                FS2TableString.SocialrelationFieldNodeName,FS2TableString.SocialrelationTableName,
                                FS2TableString.SocialrelationFieldNodeGUID),role.TongName);
                            SqlResult tongNameResult = WebUtil.QueryGameServerDb(CurrentUser.Id, server, tongNameCmd);
                            if (tongNameResult != null && tongNameResult.Success)
                            {
                                tongNameResult.SetFieldType(new SqlDataType[]{
                                    SqlDataType.String
                                });
                                object[] tongNameRecord = tongNameResult.ReadRecord();
                                if (tongNameRecord != null && tongNameRecord.Length > 0)
                                {                                    
                                    ViewState[WebConfig.ParamNodeGUID] = role.TongName;
                                    role.TongName = tongNameRecord[0] as string;
                                    //ViewState[WebConfig.ParamNodeGUID] 在role.TongName被改变后不变?!
                                }
                            }
                        }

                        return role;
					}
				}
			}
		}

		return null;
	}

    private void SetRoleBasicInfo(FS2RoleDataInfo role)
	{
		TextBoxTongName.Text = role.TongName;
		TextBoxAccountName.Text = role.AccountName;
		TextBoxRoleName.Text = role.RoleName;
		switch (role.RoleSex)
		{
			case FS2RoleSex.Male:
				TextBoxSex.Text = StringDef.Male;
				break;
			case FS2RoleSex.Female:
				TextBoxSex.Text = StringDef.Female;
				break;
		}
        string classDescription = string.Empty;
        switch (role.RoleType)
        {
            case FS2RoleType.Jiashi:
                classDescription = StringDef.Jiashi;
                break;
            case FS2RoleType.Xuanfeng:
                classDescription = StringDef.XuanFeng;
                break;
            case FS2RoleType.Xingtian:
                classDescription = StringDef.XingTian;
                break;
            case FS2RoleType.Daoshi:
                classDescription = StringDef.Daoshi;
                break;
            case FS2RoleType.Zhenren:
                classDescription = StringDef.ZhenRen;
                break;
            case FS2RoleType.Tianshi:
                classDescription = StringDef.TianShi;
                break;
            case FS2RoleType.Yiren:
                classDescription = StringDef.Yiren;
                break;
            case FS2RoleType.Shoushi:
                classDescription = StringDef.ShouShi;
                break;
            case FS2RoleType.Yishi:
                classDescription = StringDef.YiShi;
                break;
        }
        TextBoxType.Text = classDescription;

        TextBoxLevel.Text = role.RoleLevel.ToString();
        TextBoxMoney.Text = GameDataRender.RenderMoney(role.Money);
        TextBoxMoneyInBox.Text = GameDataRender.RenderMoney(role.MoneyInBox);
		TextBoxPlayedTime.Text = TimeSpan.FromSeconds(role.PlayedTime).ToString();
		TextBoxCreateDate.Text = role.CreateDate.ToString();
		TextBoxLastPlayingDate.Text = role.LastPlayingDate.ToString();
		TextBoxLastPlayingIP.Text = role.LastPlayingIP.ToString();
		TextBoxNoChatIn.Text = (role.NoChatIn == 0) ? StringDef.No : StringDef.Yes;
		TextBoxNoLoginIn.Text = (role.NoLoginIn == 0) ? StringDef.No : StringDef.Yes;
		TextBoxUseRevivePosition.Text = ((role.UseRevivePosition == 0) ? StringDef.No : StringDef.Yes);
		TextBoxReviveID.Text = role.ReviveID.ToString();
		TextBoxReviveX.Text = role.ReviveX.ToString();
		TextBoxReviveY.Text = role.ReviveY.ToString();
		TextBoxEnterMapID.Text = role.EnterMapId.ToString();
		TextBoxRoleGuid.Text = role.RoleGuid;
        LabelIsOnline.Text = role.Online == 0 ? StringDef.No : StringDef.Yes;
        ListBoxSpyLevel.SelectedIndex = role.SpyLevel;

        if (role.TongName != null && role.TongName.Length != 0)
        {
            TextBoxTongName.Text = string.Format("<a href='../GameInfo/ClanMemberStatistic.aspx?{0}={1}&{2}={3}' target='_blank' >{4}</a>",
                WebConfig.ParamServerId, (int)ViewState[WebConfig.ParamServerId], WebConfig.ParamNodeGUID, (string)ViewState[WebConfig.ParamNodeGUID], role.TongName);
            LinkClanInfo.Enabled = true;
            LinkClanInfo.NavigateUrl = string.Format("~/GameInfo/ClanMemberStatistic.aspx?{0}={1}&{2}={3}",
                WebConfig.ParamServerId, (int)ViewState[WebConfig.ParamServerId], WebConfig.ParamNodeGUID, (string)ViewState[WebConfig.ParamNodeGUID], role.TongName);
        }
        else
        {
            LinkClanInfo.Enabled = false;
            TextBoxTongName.Text = StringDef.NotAvailable;
        }

		Page.Title = string.Format("{0} - \"{1}\"", Page.Title, role.RoleName);
	}

	//private void SetRoleSkillInfo(DBRoleData role)
	//{
	//    int rowCount = TableSkill.Rows.Count;
	//    for (int i = rowCount - 1; i > 0; i--)
	//    {
	//        TableSkill.Rows.RemoveAt(i);
	//    }

	//    DBSkillData[] skills = role.SkillList;
	//    if (skills != null)
	//    {
	//        for (int i = 0; i < skills.Length; i++)
	//        {
	//            DBSkillData skill = skills[i];
	//            TableRow row = new TableRow();
	//            TableCell cell = new TableCell();
	//            LinkButton linkDel = new LinkButton();
	//            linkDel.Text = StringDef.Delete;
	//            linkDel.Click += new EventHandler(LinkButtonDeleteRoleSkill_Click);
	//            linkDel.ID = "LinkButtonDeleteRoleSkill_" + i;
	//            linkDel.Enabled = false;
	//            //TODO 暂时不提供删除技能的功能
	//            //cell.Controls.Add(linkDel);
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = (i + 1).ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.SkillId.ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.SkillLevel.ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.Status.ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.PreStatus.ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.CoolDownTime.ToString();
	//            row.Cells.Add(cell);

	//            cell = new TableCell();
	//            cell.Text = skill.PreCoolDownTime.ToString();
	//            row.Cells.Add(cell);

	//            TableSkill.Rows.Add(row);
	//        }
	//    }
	//}

	protected void LinkButtonSaveRole_Click(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(_server.SecurityObject, OpType.WRITE, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege);
		}

		//_role.TongName = TextBoxTongName.Text;
		//_role.AccountName = TextBoxAccountName.Text;
		//_role.RoleName = TextBoxRoleName.Text;
		//_role.RoleSex = (RoleSex)Enum.Parse(typeof(RoleSex), TextBoxSex.Text);
		//_role.RoleType = (RoleType)Enum.Parse(typeof(RoleType), TextBoxType.Text);
		//_role.RoleLevel = int.Parse(TextBoxLevel.Text);
		//_role.Money = int.Parse(TextBoxMoney.Text);
		//_role.MoneyInBox = int.Parse(TextBoxMoneyInBox.Text);
		//_role.PLayedTime = int.Parse(TextBoxPlayedTime.Text);
		//_role.CreateDate = DateTime.Parse(TextBoxCreateDate.Text);
		//_role.LastPlayingDate = DateTime.Parse(TextBoxLastPlayingDate.Text);
		//_role.LastPlayingIP = System.Net.IPAddress.Parse(TextBoxLastPlayingIP.Text);
		//_role.NoChatIn = int.Parse(TextBoxNoChatIn.Text);
		//_role.NoLoginIn = int.Parse(TextBoxNoLoginIn.Text);
		//_role.UseRevivePosition = int.Parse(TextBoxUseRevivePosition.Text);
		_role.ReviveID = int.Parse(TextBoxReviveID.Text);
		_role.ReviveX = int.Parse(TextBoxReviveX.Text);
		_role.ReviveY = int.Parse(TextBoxReviveY.Text);
		_role.EnterMapId = int.Parse(TextBoxEnterMapID.Text);

		SqlCommand cmd = new SqlCommand("UPDATE rolesfirst SET ReviveID='?',ReviveX='?',ReviveY='?',EnterMapID='?' WHERE Id=?",
			_role.ReviveID,
			_role.ReviveX,
			_role.ReviveY,
			_role.EnterMapId,
			//DBSkillData.ToBytes(_role.SkillList),
			_role.RoleId
			);
        if (!_server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, _server, cmd);

		if (result != null)
		{
			if (result.Success)
			{
				LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SaveRole;
			}
			else
			{
				LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SaveRole;
			}
		}
	}

	protected void LinkButtonUseRevivePos_Click(object sender, EventArgs e)
	{
		if (!WebUtil.CheckPrivilege(_server.SecurityObject, OpType.EXECUTE, Session))
		{
			Response.Redirect(WebConfig.PageNotEnoughPrivilege);
		}

		SqlCommand cmd = new SqlCommand(
			string.Format("UPDATE {0} SET {1}=1 WHERE {2}={3}",
				FS2TableString.RolesfirstTableName,
				FS2TableString.RolesfirstFieldUseRevivePosition,
				FS2TableString.RolesfirstFieldId,
				_role.RoleId)
			);
        if (!_server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
        SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, _server, cmd);

		if (result != null)
		{			
			if (result.Success)
			{
				_role.UseRevivePosition = 1;
				SetRoleBasicInfo(_role);
				LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.UseRevivePos;
			}
			else
			{
				LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.UseRevivePos;
			}
		}
	}

	protected void ListBoxSpyLevel_SelectedIndexChanged(object sender, EventArgs e)
	{
		int spyLevel = int.Parse(ListBoxSpyLevel.SelectedValue);

		SqlCommand cmd = new SqlCommand(
			string.Format("UPDATE {0} SET {1}={2} WHERE {3}={4}",
				FS2TableString.RolesfirstTableName,
				FS2TableString.RolesfirstFieldSpyLevel,
				spyLevel,
				FS2TableString.RolesfirstFieldId,
				_role.RoleId)
			);
        if (!_server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
		SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, _server, cmd);
		
		if (result != null)
		{
			if (result.Success)
			{
				_server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, _role.RoleName, string.Format("?gm ds SetSpyLevel({0})", spyLevel));
				SetRoleBasicInfo(_role);
				LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SetSpyLevel;
			}
			else
			{
				LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SetSpyLevel;
			}
		}
	}

    protected void LinkButtonGMCommand_Click(object sender, EventArgs e)
    {
        LinkButton lb = sender as LinkButton;
        if (lb != null)
        {
            switch (lb.ID.Substring(10))
            {
                case "Talk":
                    bool chatEnable;
                    if (lb.Text.Equals(StringDef.ChatDisable, StringComparison.OrdinalIgnoreCase))
                        chatEnable = false;
                    else
                        chatEnable = true;

                    if (GMUtil.SetRoleChatEnable(CurrentUser.Id, _server, _role.RoleName, chatEnable))
                    {
                        if (chatEnable) lb.Text = StringDef.ChatDisable;
                        else lb.Text = StringDef.ChatEnable;

                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + (chatEnable ? StringDef.ChatEnable : StringDef.ChatDisable);
                    }
                    else
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + (chatEnable ? StringDef.ChatEnable : StringDef.ChatDisable);

                    break;
                case "Freeze":
                    bool freeze;
                    if (lb.Text.Equals(StringDef.Freeze, StringComparison.OrdinalIgnoreCase))
                        freeze = true;
                    else
                        freeze = false;

                    if (GMUtil.SetRoleFrozen(CurrentUser.Id, _server, _role.RoleName, freeze))
                    {
                        if (freeze) lb.Text = StringDef.Unfreeze;
                        else lb.Text = StringDef.Freeze;
                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + (freeze ? StringDef.Freeze : StringDef.Unfreeze);
                    }
                    else
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + (freeze ? StringDef.Freeze : StringDef.Unfreeze);

                    break;
                case "Save":
                    if (_server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid,LordControl.ActionKeyExecuteGMCommand, _role.RoleName, WebConfig.GMCommandSave))
                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.SaveByForce;
                    else
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.SaveByForce;
                        break;
                case "Quit":                    
                    if (_server.DoPlugInAction(CurrentUser.Id, LordControl.PlugInGuid, LordControl.ActionKeyExecuteGMCommand, _role.RoleName, WebConfig.GMCommandKickPlayer))
                        LabelOpMsg.Text = StringDef.OperationSucceed + StringDef.Colon + StringDef.QuitByForce;
                    else
                        LabelOpMsg.Text = StringDef.OperationFail + StringDef.Colon + StringDef.QuitByForce;
                    break;
            }
        }
    }
}