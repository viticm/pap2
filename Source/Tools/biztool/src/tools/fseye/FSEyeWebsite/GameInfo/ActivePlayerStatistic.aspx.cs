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
using FSEye.FS2Lib;
using FSEye.Security;

public partial class GameInfo_ActivePlayerStatistic : BasePage
{
    private const int MinLevelValue = 1;
    private const int MaxLevelValue = 120;
    private const int DefaultRecordPerPage = 50;
    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameInfoStatisticActivePlayerStatistic, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);            
            CalendarDate.SelectedDate = DateTime.Today;
            CreateCheckBoxListRoleType();
        }
        else
        {
            try
            {
                _recordPerPage = int.Parse(ListBoxRecordPerPage.SelectedValue);
            }
            catch (Exception)
            {
                _recordPerPage = DefaultRecordPerPage;
            }
        }

        PanelResult.Visible = false;
        LabelOpMsg.Text = string.Empty;
    }

    private void CreateCheckBoxListRoleType()
    {
        ListItem item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.ProfessionalNotChoose), "0,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.XuanFeng), "0,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Jiashi, StringDef.XingTian), "0,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.ProfessionalNotChoose), "1,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.ZhenRen), "1,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Daoshi, StringDef.TianShi), "1,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.ProfessionalNotChoose), "2,-1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.ShouShi), "2,0");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
        item = new ListItem(string.Format("{0}[{1}]", StringDef.Yiren, StringDef.YiShi), "2,1");
        item.Selected = true;
        CheckBoxListRoleType.Items.Add(item);
    }

    protected void ServerGroupIndexChanged(object sender, EventArgs e)
    {
        ServerGroup group = ServerGroupDropDownList.SelectedServerGroup;
        if (group != null) ServerDropDownList.CreateServerList(group);
    }

    protected void StatisticTypeIndexChanged(object sender, EventArgs e)
    {
        if (StatisticType.SelectedIndex == 0)
        {
            ListBoxOrderBy.Items[0].Enabled = true;
        }
        else ListBoxOrderBy.Items[0].Enabled = false;
    }

    protected void ButtonStatistic_Click(object sender, EventArgs e)
    {
        Query(0);
    }

    void Query(int offset)
    {
        try
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
            ArrayList paramList = new ArrayList();
            StringBuilder searchCondition = new StringBuilder();

            StringBuilder roleTypeBuilder = new StringBuilder();
            foreach (ListItem item in CheckBoxListRoleType.Items)
            {
                if (item.Selected)
                {
                    string[] conValue = item.Value.Split(',');
                    roleTypeBuilder.AppendFormat("({0}='{1}' AND {2}='{3}') OR ", FS2TableString.RolesfirstFieldRoleType, conValue[0], FS2TableString.RolesfirstFieldSkillSeries, conValue[1]);
                }
            }
            if (roleTypeBuilder.Length != 0)
            {
                searchCondition.AppendFormat(" AND ({0}) ", roleTypeBuilder.ToString(0, roleTypeBuilder.Length - 3));
            }
            else
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.RoleClass);
                return;
            }

            int startLevel = int.Parse(TextBoxStartLevel.Text);
            if (startLevel > 1)
            {
                searchCondition.Append(string.Format(" AND {0}>={1}", FS2TableString.RolesfirstFieldRoleLevel, startLevel));
            }

            int endLevel = int.Parse(TextBoxEndLevel.Text);
            if (endLevel < FS2GameDataManager.MaxLevel)
            {
                searchCondition.Append(string.Format(" AND {0}<={1}", FS2TableString.RolesfirstFieldRoleLevel, endLevel));
            }

            DateTime activeDate = CalendarDate.SelectedDate;
            if (activeDate == DateTime.MinValue)
            {
                LabelOpMsg.Text = string.Format(StringDef.MsgInputParamNotCorrect, StringDef.Date);
                return;
            }
            switch (StatisticType.SelectedIndex)
            {
                case -1:
                    LabelOpMsg.Text = string.Format(StringDef.MsgCannotBeNone, StringDef.Statistic + StringDef.Type);
                    return;
                case 0:
                    //活跃玩家
                    searchCondition.AppendFormat(" AND {0}>='{1}' ", FS2TableString.RolesfirstFieldLastPlayingDate, activeDate.AddDays(-WebConfig.ActivePlayerDaySpan));
                    break;
                case 1:
                    //不活跃玩家
                    searchCondition.AppendFormat(" AND {0}<'{1}' ", FS2TableString.RolesfirstFieldLastPlayingDate, activeDate.AddDays(-WebConfig.ActivePlayerDaySpan));
                    break;
                case 2:
                    //流失玩家
                    searchCondition.AppendFormat(" AND {0}>='{1}' AND {0}<'{2}' ", FS2TableString.RolesfirstFieldLastPlayingDate, activeDate.AddDays(-WebConfig.ActivePlayerDaySpan).AddDays(-WebConfig.ActivePlayerDayValue), activeDate.AddDays(-WebConfig.ActivePlayerDaySpan));
                    break;
            }

            if (searchCondition.Length > 0)
            {
                searchCondition.Remove(0, 4);
                searchCondition.Insert(0, " WHERE");
            }

            string orderByType = string.Empty;
            switch (ListBoxOrderByType.SelectedIndex)
            {
                case 0:
                    orderByType = "ASC";
                    break;
                case 1:
                    orderByType = "DESC";
                    break;
            }

            string orderByStatement = string.Empty;
            switch (ListBoxOrderBy.SelectedIndex)
            {
                case 0:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.RolesfirstFieldRoleName, orderByType);
                    break;
                case 1:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.RolesfirstFieldRoleLevel, orderByType);
                    break;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount);

            string cmdText = "SELECT {0} FROM {1} {2} {3} {4}";
            string cmdFieldText = "Id,AccountName,RoleName,RoleSex,RoleType,RoleLevel,SkillSeries";

            SqlCommand cmd = new SqlCommand(string.Format(cmdText, cmdFieldText, FS2TableString.RolesfirstTableName,
                            searchCondition.ToString(), orderByStatement, limitStatement));
            SqlResult result = WebUtil.QueryGameServerDb(CurrentUser.Id, server, cmd);
            if (result != null && result.Success)
            {
                Session[WebConfig.SessionQueryLogOffset] = offset;

                result.SetFieldType(
                new SqlDataType[] {
                            SqlDataType.Int32,
							SqlDataType.String,
							SqlDataType.String,
							SqlDataType.SByte,
							SqlDataType.SByte,
							SqlDataType.SByte,
                            SqlDataType.Int32
						});

                ArrayList roleList = new ArrayList();
                object[] record = null;
                while ((record = result.ReadRecord()) != null)
                {
                    FS2RoleDataInfo role = new FS2RoleDataInfo();
                    role.RoleId = (int)record[0];
                    role.AccountName = record[1] as string;
                    role.RoleName = record[2] as string;
                    role.RoleLevel = (SByte)record[5];
                    role.SkillSeries = (int)record[6];
                    switch ((SByte)record[4])
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

                    roleList.Add(role);
                }

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (roleList.Count >= limitCount);

                bool success = CreateSearchRoleResultList(server.Id, (FS2RoleDataInfo[])roleList.ToArray(typeof(FS2RoleDataInfo)));
                PanelResult.Visible = success;

                if (!success)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                }
                else
                {
                    LabelResult.Text = string.Format(StringDef.LabelStatisticResult, server.Group.Name, server.Name,
                                            string.Format(StringDef.LabelRoleLevel, startLevel, endLevel),
                                            StatisticType.SelectedItem.Text);
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
        catch (Exception ex)
        {
            LabelOpMsg.Text = ex.Message;
            PanelResult.Visible = false;
        }
    }

    private bool CreateSearchRoleResultList(int serverId, FS2RoleDataInfo[] roles)
    {
        if (roles != null && roles.Length > 0)
        {
            for (int i = 0; i < roles.Length; i++)
            {
                FS2RoleDataInfo info = roles[i];
                TableRow row = new TableRow();

                TableCell cell = new TableCell();
                HyperLink link = new HyperLink();
                link.Target = "_blank";
                link.NavigateUrl = "~/GameMaster/RoleDetail.aspx?serverId=" + serverId + "&roleId=" + info.RoleId.ToString();
                link.Text = info.RoleName;
                link.SkinID = "PlainText";
                cell.Controls.Add(link);
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.AccountName;
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.RoleLevel.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                string classDescription = string.Empty;
                switch (info.RoleType)
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
                cell.Text = classDescription;
                row.Cells.Add(cell);

                cell = new TableCell();

                TableSearchRoleList.Rows.Add(row);
            }
            return true;
        }

        
        return false;
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

    void CreateLostRate(int lostNum, int lastWeekActiveNum)
    {
        if (lastWeekActiveNum != 0)
        {
            TableRow row = new TableRow();
            row.Font.Bold = true;
            TableCell cell = new TableCell();
            cell.Text = "流失玩家数量: " + lostNum.ToString();
            cell.Text += " 流失率: " + Decimal.Round((decimal)lostNum / lastWeekActiveNum * 100, 2).ToString() + "%";
            cell.ColumnSpan = 5;
            row.Cells.Add(cell);
            TableSearchRoleList.Rows.Add(row);
        }
    }
}
