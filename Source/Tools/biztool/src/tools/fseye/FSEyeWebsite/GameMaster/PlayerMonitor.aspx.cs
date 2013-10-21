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
using System.Text;
using Resources;
using FSEye;
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameMaster_PlayerMonitor : BasePage
{
	const int DefaultRecordPerPage = 50;

	int _recordPerPage = DefaultRecordPerPage;

	protected void Page_Load(object sender, EventArgs e)
	{
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterPlayerMonitor, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

		if (!IsPostBack)
		{
			CreatePlayerActionList();
			ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);

            ScriptCalendarStartTime.SelectedDate = DateTime.Today;
            ScriptCalendarEndTime.SelectedDate = DateTime.Now;
		}
        LabelOpMsg.Text = string.Empty;
	}

	void CreatePlayerActionList()
	{
		CheckBoxListPlayerAction.Items.Clear();
		IList<FS2PlayerActionData> actionList = FS2GameDataManager.TheInstance.GetAllPlayerActions();
		foreach (FS2PlayerActionData actionInfo in actionList)
		{
			ListItem item = new ListItem(actionInfo.ToString(), actionInfo.Id.ToString());			
			CheckBoxListPlayerAction.Items.Add(item);
			item.Selected = true;
		}
	}

	protected void SelectedServerGroupChanged(object sender, EventArgs e)
	{
		ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
		if (group != null) ServerDropDownList.CreateServerList(group);
	}

	protected void LinkButtonSearchRole_Click(object sender, EventArgs e)
	{
		Query(0);
	}

	protected void ButtonFirstPage_Click(object sender, EventArgs e)
	{
		Query(0);
	}

	protected void ButtonPreviousPage_Click(object sender, EventArgs e)
	{
		int offset = (int)Session[WebConfig.SessionQueryLogOffset];
		if (offset > 0)
		{
			offset -= _recordPerPage;
		}
		if (offset < 0)
			offset = 0;

		Query(offset);
	}

	protected void ButtonNextPage_Click(object sender, EventArgs e)
	{
		int offset = (int)Session[WebConfig.SessionQueryLogOffset];
		offset += _recordPerPage;

		Query(offset);
	}

	class PlayerActionInfo
	{
		public int Id;
		public DateTime RecordTime;
		public int Action;
		public string Desc;
	}

	void Query(int offset)
	{
		GameServer server = ServerDropDownList.SelectedGameServer;
        if (server == null)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.GameServer);
            return;
        }
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }

		string roleName = TextBoxRole.Text.Trim();
        if (roleName == null || roleName.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.RoleName);
            return;
        }

		string roleGuid = WebUtil.FindRoleGuidByName(CurrentUser.Id, server, roleName);
        if (roleGuid == null || roleGuid.Length == 0)
        {
            LabelOpMsg.Text = string.Format(StringDef.MsgNotExist, StringDef.RoleName);
            return;
        }

		StringBuilder whereStatement = new StringBuilder();
		whereStatement.AppendFormat(" WHERE PlayerGuid='{0}'", roleGuid);

		foreach (ListItem item in CheckBoxListPlayerAction.Items)
		{
			if (!item.Selected)
			{
				whereStatement.AppendFormat(" AND ActionId<>{0}", item.Value);
			}
		}

		if (ScriptCalendarStartTime.Selected)
		{
			DateTime startTime = ScriptCalendarStartTime.SelectedDate;
			whereStatement.AppendFormat(" AND RecordTime>='{0}'", startTime);
		}

		if (ScriptCalendarEndTime.Selected)
		{
			DateTime endTime = ScriptCalendarEndTime.SelectedDate;
			whereStatement.AppendFormat(" AND RecordTime<='{0}'", endTime);
		}

		StringBuilder orderByStatement = new StringBuilder();
		if (ListBoxOrderBy.SelectedIndex == 0)
		{
			orderByStatement.Append(" ORDER BY RecordTime");
		}
		else
		{
			orderByStatement.Append(" ORDER BY PlayerAction");
		}
		if (ListBoxOrderByType.SelectedIndex == 0)
		{
			orderByStatement.Append(" ASC");
		}
		else
		{
			orderByStatement.Append(" DESC");
		}

		SqlCommand cmd = new SqlCommand("SELECT Id,RecordTime,PlayerAction,ActionDesc FROM playeraction" + whereStatement.ToString() + orderByStatement.ToString());
        if (!server.IsConnected)
        {
            LabelOpMsg.Text = StringDef.NoConnectionAlert;
            return;
        }
		SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);

		ArrayList c2sNetMsgInfoList = new ArrayList();
        if (result != null && result.Success)
        {
            result.SetFieldType(new SqlDataType[] { SqlDataType.Int32, SqlDataType.DateTime, SqlDataType.Int32, SqlDataType.String });
            object[] record = null;
            while ((record = result.ReadRecord()) != null)
            {
                PlayerActionInfo info = new PlayerActionInfo();
                info.Id = (int)record[0];
                info.RecordTime = (DateTime)record[1];
                info.Action = (int)record[2];
                info.Desc = record[3] as string;
                c2sNetMsgInfoList.Add(info);
            }

            if (c2sNetMsgInfoList.Count == 0)
            {
                LabelOpMsg.Text = StringDef.NoMatchingRecord;
                return;
            }
            else
            {
                CreateTableC2SNetMsg(c2sNetMsgInfoList);
                PanelResult.Visible = true;
            }
        }
        else
        {
            if (result == null)
                LabelOpMsg.Text = StringDef.QueryTimeOut;
            else
                LabelOpMsg.Text = StringDef.OperationFail;
        }
	}

	void CreateTableC2SNetMsg(ArrayList c2sNetMsgInfoList)
	{
		if (c2sNetMsgInfoList == null)
			return;

		foreach (PlayerActionInfo info in c2sNetMsgInfoList)
		{
			TableRow row = new TableRow();
			TableCell cell = new TableCell();
			cell.Text = info.Id.ToString();
			row.Cells.Add(cell);

			cell = new TableCell();
			cell.Text = info.RecordTime.ToString();
			row.Cells.Add(cell);

			cell = new TableCell();
			cell.Text = info.Action.ToString();
			row.Cells.Add(cell);

			cell = new TableCell();
            if (info.Desc.Contains("skill"))
            {
                string skillDesc = info.Desc.Trim('<','>');
                int skillTemplateId = int.Parse(skillDesc.Substring(skillDesc.IndexOf('_') + 1));
                FS2SkillData skillData = FS2GameDataManager.TheInstance.GetSkillData(skillTemplateId);
                if (skillData != null)
                {
                    cell.Text = info.Desc.Substring(0, info.Desc.IndexOf('<')) + string.Format("&lt;<span onclick='{0}'>{1}</span>>",
                        string.Format("updateRoleSkillInfo(\"{0}\")", skillData.TemplateId),
                        skillData.Name);
                }
                else
                {
                    cell.Text = info.Desc.Substring(0, info.Desc.IndexOf('<')) + info.Desc.Replace("<", "&lt;");
                }
            }
            else
                cell.Text = info.Desc;
			row.Cells.Add(cell);

			TableC2SNetMsg.Rows.Add(row);
		}
	}
}