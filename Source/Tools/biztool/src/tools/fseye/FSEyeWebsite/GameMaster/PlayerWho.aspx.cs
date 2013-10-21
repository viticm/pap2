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
using FSEye.PlugIn;
using FSEye.FS2Lib;
using System.Text;

public partial class GameMaster_PlayerWho : BasePage
{
    const int DefaultRecordPerPage = 50;

    int _recordPerPage = DefaultRecordPerPage;

    protected void Page_Load(object sender, EventArgs e)
    {
        if (!WebUtil.CheckPrivilege(WebConfig.FunctionGameMasterPlayerWho, OpType.READ, Session))
        {
            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
        }

        if (!IsPostBack)
        {
            //CreateLevelList();
            ListBoxRecordPerPage.SelectedIndex = 1;
            _recordPerPage = DefaultRecordPerPage;
            ServerDropDownList.CreateServerList(ServerGroupDropDownList.SelectedServerGroup);
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
        if(group!=null)ServerDropDownList.CreateServerList(group);
    }

    protected void ButtonRefresh_Click(object sender, EventArgs e)
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

            //string tong = TextBoxTong.Text;
            //if (tong.Length > 0)
            //{
            //    if (CheckBoxTong.Checked)
            //    {
            //        searchCondition.Append(string.Format(" AND {0}='?'", FS2TableString.RolesfirstFieldTongGUID));
            //    }
            //    else
            //    {
            //        searchCondition.Append(string.Format(" AND {0} LIKE '%?%'", FS2TableString.RolesfirstFieldTongGUID));
            //    }
            //    paramList.Add(tong);
            //}

            searchCondition.Append(string.Format("{0} = {1}", " WHERE " + FS2TableString.RolesfirstFieldOnline, "1"));

            int sex = ListBoxSex.SelectedIndex - 1;
            if (sex >= 0)
            {
                searchCondition.Append(string.Format(" AND {0}={1}", FS2TableString.RolesfirstFieldRoleSex, sex));
            }

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

            //int roleClass = ListBoxRoleClass.SelectedIndex - 1;
            //if (roleClass >= 0)
            //{
            //    string[] conValue = ListBoxRoleClass.SelectedValue.Split(',');
            //    searchCondition.Append(string.Format(" AND {0}='{1}' AND {2}='{3}'", FS2TableString.RolesfirstFieldRoleType, conValue[0], FS2TableString.RolesfirstFieldSkillSeries, conValue[1]));
            //}

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
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.RolesfirstFieldId, orderByType);
                    break;
                case 1:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.RolesfirstFieldRoleLevel, orderByType);
                    break;
                case 2:
                    orderByStatement = string.Format(" ORDER BY {0} {1}", FS2TableString.RolesfirstFieldRoleName, orderByType);
                    break;
            }

            int limitCount = _recordPerPage;
            string limitStatement = string.Format(" LIMIT {0},{1}", offset, limitCount + 1);

            SqlCommand cmd = new SqlCommand("SELECT Id,AccountName,RoleName,RoleSex,RoleType,RoleLevel,SkillSeries FROM rolesfirst" + searchCondition.ToString() + orderByStatement + limitStatement, paramList.ToArray());
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

                ArrayList roles = new ArrayList();
                object[] record = null;
                while ((record = result.ReadRecord()) != null)
                {
                    FS2RoleDataInfo info = new FS2RoleDataInfo();
                    info.RoleId = (int)record[0];
                    info.AccountName = (string)record[1];
                    info.RoleName = (string)record[2];
                    info.RoleSex = (FS2RoleSex)((SByte)record[3]);

                    info.SkillSeries = (int)record[6];
                    switch ((SByte)record[4])
                    {
                        case (SByte)0:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Xuanfeng;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Xingtian;
                            else info.RoleType = FS2RoleType.Jiashi;
                            break;
                        case (SByte)1:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Zhenren;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Tianshi;
                            else info.RoleType = FS2RoleType.Daoshi;
                            break;
                        case (SByte)2:
                            if (info.SkillSeries == 0) info.RoleType = FS2RoleType.Shoushi;
                            else if (info.SkillSeries == 1) info.RoleType = FS2RoleType.Yishi;
                            else info.RoleType = FS2RoleType.Yiren;
                            break;
                    }

                    info.RoleLevel = (SByte)record[5];

                    roles.Add(info);
                }

                ButtonPreviousPage.Enabled = (offset > 0);
                ButtonFirstPage.Enabled = (offset > 0);
                ButtonNextPage.Enabled = (roles.Count > limitCount);

                bool success = CreateSearchRoleResultList(server.Id, (FS2RoleDataInfo[])roles.ToArray(typeof(FS2RoleDataInfo)));
                PanelResult.Visible = success;

                if (!success)
                {
                    LabelOpMsg.Text = StringDef.NoMatchingRecord;
                    return;
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
            FS2RoleDataInfo info = null;
            for (int i = 0; i < roles.Length; i++)
            {
                info = roles[i];
                TableRow row = new TableRow();

                TableCell cell = new TableCell();
                cell.Text = info.RoleId.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                HyperLink link = new HyperLink();
                link.Target = "_blank";
                link.NavigateUrl = "RoleDetail.aspx?serverId=" + serverId + "&roleId=" + info.RoleId;
                link.Text = info.RoleName;
                link.SkinID = "PlainText";
                cell.Controls.Add(link);
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Text = info.AccountName;
                row.Cells.Add(cell);

                cell = new TableCell();
                if (info.TongName != null && info.TongName.Length != 0)
                {
                    cell.Text = info.TongName;
                }
                else
                {
                    cell.Text = StringDef.None;
                }
                row.Cells.Add(cell);

                cell = new TableCell();
                cell.Font.Bold = true;
                cell.Text = info.RoleLevel.ToString();
                row.Cells.Add(cell);

                cell = new TableCell();
                Image sexImage = new Image();
                switch (info.RoleSex)
                {
                    case FS2RoleSex.Male:
                        sexImage.ToolTip = StringDef.Male;
                        sexImage.ImageUrl = "~/Image/male.gif";
                        break;
                    case FS2RoleSex.Female:
                        sexImage.ToolTip = StringDef.Female;
                        sexImage.ImageUrl = "~/Image/female.gif";
                        break;

                }
                cell.Controls.Add(sexImage);
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
                cell.Wrap = false;
                link = new HyperLink();
                link.Text = StringDef.ExecuteGMCommand;
                link.SkinID = "PlainText";
                link.NavigateUrl = "GMCommand.aspx?serverId=" + serverId + "&roleName=" + Server.UrlEncode(info.RoleName);
                link.Width = new Unit(40,UnitType.Percentage);
                cell.Controls.Add(link);
                link = new HyperLink();
                link.Text = StringDef.ExpStatistic;
                link.SkinID = "PlainText";
                link.NavigateUrl = "~/GameInfo/ExpStatistic.aspx?serverId=" + serverId + "&roleName=" + Server.UrlEncode(info.RoleName);
                link.Width = new Unit(40, UnitType.Percentage);
                cell.Controls.Add(link);
                row.Cells.Add(cell);

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


    //void CreateLevelList()
    //{
    //    for (int level = 1; level <= FS2GameDataManager.MaxLevel; level++)
    //    {
    //        ListBoxStartLevel.Items.Add(new ListItem(level.ToString(), level.ToString()));
    //        ListBoxEndLevel.Items.Add(new ListItem(level.ToString(), level.ToString()));
    //    }

    //    ListBoxStartLevel.SelectedIndex = 0;
    //    ListBoxEndLevel.SelectedIndex = ListBoxEndLevel.Items.Count - 1;
    //}


    //protected void ServerIndexChanged(object sender, EventArgs e)
    //{
    //    CreatePlayerWhoTable();
    //}

    //protected void ButtonRefresh_Click(object sender, EventArgs e)
    //{
    //    PlayerWho();
    //}

    //private void CreatePlayerWhoTable()
    //{
    //    GameServer server = ServerDropDownList1.SelectedGameServer;
    //    if (server != null)
    //    {
    //        if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.READ, Session))
    //        {
    //            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //        }

    //        IList playerWhoList = server.GetPlugInData(CurrentUser.Id,LordControl.PlugInGuid, LordControl.DataKeyPlayerWho) as IList;
    //        foreach (PlayerInfo info in playerWhoList)
    //        {
    //            TableRow row = new TableRow();

    //            TableCell cell = new TableCell();
    //            cell.Text = info.Name;
    //            row.Cells.Add(cell);

    //            TablePlayer.Rows.Add(row);
    //        }
    //    }
    //}

    //private void PlayerWho()
    //{
    //    GameServer server = ServerDropDownList1.SelectedGameServer;
    //    if (server != null)
    //    {
    //        if (!WebUtil.CheckPrivilege(server.SecurityObject, OpType.EXECUTE, Session))
    //        {
    //            Response.Redirect(WebConfig.PageNotEnoughPrivilege, true);
    //        }

    //        try
    //        {
    //            ushort offset = ushort.Parse(TextBoxOffset.Text);
    //            if (server.DoPlugInAction(CurrentUser.Id,LordControl.PlugInGuid, LordControl.ActionKeyRefreshPlayerWho, offset))
    //            {
    //                ShowMessage(StringDef.Success, MessageType.Success);

    //                //这里延迟一会儿是为了等待数据更新
    //                System.Threading.Thread.Sleep(RefreshWaitMilliseconds);

    //                CreatePlayerWhoTable();
    //            }
    //            else
    //            {
    //                ShowMessage(StringDef.Failure, MessageType.Failure);
    //            }
    //        }
    //        catch (Exception e)
    //        {
    //            ShowMessage(StringDef.Failure + StringDef.Colon + e.Message, MessageType.Failure);
    //        }
    //    }
    //}
}
